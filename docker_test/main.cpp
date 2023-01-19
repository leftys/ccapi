#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#include "ccapi_cpp/ccapi_session.h"
namespace ccapi {
Logger* Logger::logger = nullptr;  // This line is needed.
class MyEventHandler : public EventHandler {
 public:
  bool processEvent(const Event& event, Session* session) override {
    if (event.getType() == Event::Type::SUBSCRIPTION_DATA) {
      for (const auto& message : event.getMessageList()) {
        for (const auto& element : message.getElementList()) {
          std::lock_guard<std::mutex> lock(m);
          if (element.has("BID_PRICE")) {
            bestBidPrice = element.getValue("BID_PRICE");
          }
          if (element.has("ASK_PRICE")) {
            bestAskPrice = element.getValue("ASK_PRICE");
          }
        }
      }
    }
    else if (event.getType() == Event::Type::RESPONSE) {
      bool is_handled = false;
      for (const auto& message: event.getMessageList()) {
        if (message.getType() == Message::Type::RESPONSE_ERROR) {
          for (const auto& element : message.getElementList()) {
            // std::cout << "Received at " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
            std::cout << "Error: " << element.toString() << std::endl;
            is_handled = true;
          }
        }
        if (message.getType() == Message::Type::CREATE_ORDER) {
          // std::cout << "Created at " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
          const auto t = std::chrono::high_resolution_clock::now();
          std::cout << "Create latency " << std::chrono::duration_cast<std::chrono::microseconds>(t - time_orders_sent).count() << "us" << std::endl;
        }
        if (message.getType() == Message::Type::CANCEL_OPEN_ORDERS) {
          // std::cout << "Canceled at " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
          const auto t = std::chrono::high_resolution_clock::now();
          std::cout << "Cancel latency " << std::chrono::duration_cast<std::chrono::microseconds>(t - time_cancel_sent).count() << "us" << std::endl;
        }
      }
      // if (!is_handled) {
      // }
    } else {
      // std::cout << "Received unknown message " << event.toStringPretty(2, 2);
    }
    return true;
  }
  std::pair<std::string, std::string> getBBO() {
    std::lock_guard<std::mutex> lock(m);
    return std::make_pair(bestBidPrice, bestAskPrice);
  }
  // TODO this is not thread-safe!
  std::chrono::time_point<std::chrono::high_resolution_clock> time_orders_sent;
  std::chrono::time_point<std::chrono::high_resolution_clock> time_cancel_sent;

 private:
  mutable std::mutex m;
  std::string bestBidPrice;
  std::string bestAskPrice;
};
} /* namespace ccapi */
std::string roundPrice(double price) {
  std::stringstream stream;
  stream << std::fixed << std::setprecision(2) << price;
  return stream.str();
}
bool should_exit = false;
void sigint_handler(int s) {
  std::cout << "Caught signal " << s << std::endl;
  should_exit = true;
}
using ::ccapi::MyEventHandler;
using ::ccapi::Request;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;
using ::ccapi::Subscription;
using ::ccapi::UtilSystem;
int main(int argc, char** argv) {
  if (argc != 3) {
    std::cerr << "Usage: <program name> <spread proportion> <order quantity>\n"
              << "Example:\n"
              << "    main 0.5 0.0001" << std::endl;
    return EXIT_FAILURE;
  }
  double spreadProportion = std::stod(argv[1]);
  std::string orderQuantity = argv[2];
  if (UtilSystem::getEnvAsString("BINANCE_API_KEY").empty()) {
    std::cerr << "Please set environment variable BINANCE_API_KEY" << std::endl;
    return EXIT_FAILURE;
  }
  if (UtilSystem::getEnvAsString("BINANCE_API_SECRET").empty()) {
    std::cerr << "Please set environment variable BINANCE_API_SECRET" << std::endl;
    return EXIT_FAILURE;
  }
  std::map<std::string, std::string> myCredentials = {
    {CCAPI_BINANCE_API_KEY, UtilSystem::getEnvAsString("BINANCE_API_KEY")},
    {CCAPI_BINANCE_API_SECRET, UtilSystem::getEnvAsString("BINANCE_API_SECRET")}
  };
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = sigint_handler;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);
  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  MyEventHandler eventHandler;
  Session session(sessionOptions, sessionConfigs, &eventHandler);
  Subscription subscription("binance", "BTCUSDT", "MARKET_DEPTH");
  session.subscribe(subscription);
  while (!should_exit) {
    auto bbo = eventHandler.getBBO();
    if (!bbo.first.empty() && !bbo.second.empty()) {
      double midPrice = (std::stod(bbo.first) + std::stod(bbo.second)) / 2;
      std::cout << "Current mid price is " + std::to_string(midPrice) << std::endl;
      std::string buyPrice = roundPrice(midPrice * (1 - spreadProportion / 2));
      std::string sellPrice = roundPrice(midPrice * (1 + spreadProportion / 2));
      std::vector<Request> requestList;
      Request requestBuy(Request::Operation::CREATE_ORDER, "binance", "BTCUSDT", "", myCredentials);
      requestBuy.appendParam({
          {"SIDE", "BUY"},
          {"QUANTITY", orderQuantity},
          {"LIMIT_PRICE", buyPrice},
      });
      requestList.push_back(requestBuy);
      Request requestSell(Request::Operation::CREATE_ORDER, "binance", "BTCUSDT", "", myCredentials);
      requestSell.appendParam({
          {"SIDE", "SELL"},
          {"QUANTITY", orderQuantity},
          {"LIMIT_PRICE", sellPrice},
      });
      requestList.push_back(requestSell);
      // std::cout << "Sending at " << std::chrono::high_resolution_clock::now().time_since_epoch().count() << std::endl;
      eventHandler.time_orders_sent = std::chrono::high_resolution_clock::now();
      session.sendRequest(requestList);
      std::cout << "Buy " + orderQuantity + " BTCUSDT at price " + buyPrice << std::endl;
      std::cout << "Sell " + orderQuantity + " BTCUSDT at price " + sellPrice << std::endl;
    } else {
      std::cout << "Insufficient market information" << std::endl;
    }
    int timeToSleepSeconds = 3;
    std::cout << "About to sleep for " + std::to_string(timeToSleepSeconds) + " seconds\n" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(timeToSleepSeconds));
    Request requestCancel(Request::Operation::CANCEL_OPEN_ORDERS, "binance", "BTCUSDT", "", myCredentials);
    eventHandler.time_cancel_sent = std::chrono::high_resolution_clock::now();
    session.sendRequest(requestCancel);
  }
  session.stop();
  std::cout << "Exitted" << std::endl;
  return EXIT_SUCCESS;
}
