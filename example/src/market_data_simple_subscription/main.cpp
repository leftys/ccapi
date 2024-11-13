#include "ccapi_cpp/ccapi_session.h"
#include <chrono>
namespace ccapi {
class MyEventHandler : public EventHandler {
 public:
  bool processEvent(const Event& event, Session* session) override {
    if (event.getType() == Event::Type::SUBSCRIPTION_STATUS) {
      std::cout << "Received an event of type SUBSCRIPTION_STATUS:\n" + event.toStringPretty(2, 2) << std::endl;
    } else if (event.getType() == Event::Type::SUBSCRIPTION_DATA) {
      for (const auto& message : event.getMessageList()) {
        if (message.getType() == Message::Type::MARKET_DATA_EVENTS_MARKET_DEPTH) {
          std::cout << std::string("Order book at ") + UtilTime::getISOTimestamp(message.getTime()) + " are:" << std::endl;
          for (const auto& element : message.getElementList()) {
            const std::map<std::string, std::string>& elementNameValueMap = element.getNameValueMap();
            std::cout << "  " + toString(elementNameValueMap) << std::endl;
          }
        }
        else if (message.getType() == Message::Type::MARKET_DATA_EVENTS_TRADE) {
          std::cout << std::string("First trade at ") + UtilTime::getISOTimestamp(message.getTime()) + " are:" << std::endl;
          for (const auto& element : message.getElementList()) {
            const std::map<std::string, std::string>& elementNameValueMap = element.getNameValueMap();
            std::cout << "  " + toString(elementNameValueMap) << std::endl;
            break;
          }
        }
      }
    } else if (event.getType() == Event::Type::RESPONSE) {
      std::cout << "Received an event of type RESPONSE:\n" + event.toStringPretty(2, 2) << std::endl;
    } else {
      std::cout << "Received an unknown event:\n" + event.toStringPretty(2, 2) << std::endl;
    }
    return true;
  }
};

class MyLogger final : public Logger {
 public:
  void logMessage(const std::string& severity, const std::string& threadId, const std::string& timeISO, const std::string& fileName,
                  const std::string& lineNumber, const std::string& message) override {
    // if (severity != "DEBUG" && severity != "TRACE") {
    std::cout << severity << " "  << fileName << ":" << lineNumber << " " << message << std::endl;
    // }
  }
};
MyLogger myLogger;
// Logger* Logger::logger = nullptr;  // This line is needed.
Logger* Logger::logger = &myLogger;

} /* namespace ccapi */

using ::ccapi::MyEventHandler;
using ::ccapi::Session;
using ::ccapi::SessionConfigs;
using ::ccapi::SessionOptions;
using ::ccapi::Subscription;
using ::ccapi::Request;
using ::ccapi::toString;
int main(int argc, char** argv) {
  // auto now = std::chrono::high_resolution_clock::now();
  // auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
  if (argc != 3) {
    std::cerr << "Usage: <program name> <exchange name> <symbol name>\n"
              << "Example:\n"
              << "    main huobi XCAD_USDT" << std::endl;
    return EXIT_FAILURE;
  }
  std::string exchangeName = argv[1];
  std::string symbolName = argv[2];

  SessionOptions sessionOptions;
  SessionConfigs sessionConfigs;
  std::map<std::string, std::string> credentials = {
      {"HUOBI_API_KEY", "asdsd"},
      {"HUOBI_API_SECRET", "erfdsfd"},
  };
  sessionConfigs.setCredential(credentials);
  MyEventHandler eventHandler;
  Session session(sessionOptions, sessionConfigs, &eventHandler);
  Subscription subscription(exchangeName, symbolName, "MARKET_DEPTH", "MARKET_DEPTH_RETURN_UPDATE=1&CONFLATE_INTERVAL_MILLISECONDS=100&MARKET_DEPTH_MAX=100",
                            "depth_corr_id");
  Subscription subscription2(exchangeName, symbolName, "TRADE", "",
                            "trades_corr_id");
  // Subscription subscription("gateio", "XCAD_USDT", "GENERIC_PUBLIC_SUBSCRIPTION", R"({"time": timestamp, "channel": "spot.order_book_update",
  // "event":"subscribe","payload":["XCAD_USDT", "100ms"]})");
  session.subscribe(subscription);
  session.subscribe(subscription2);

  // Request request(Request::Operation::GET_MARKET_DEPTH, exchangeName, symbolName);
  // request.appendParam({
  //     {"LIMIT", "20"},
  // });
  // session.sendRequest(request);

  if (exchangeName == "gateio") {
    Request request(Request::Operation::GENERIC_PUBLIC_REQUEST, exchangeName);
    request.appendParam({
        {"HTTP_METHOD", "GET"},
        {"HTTP_PATH", "/api/v4/spot/order_book"},
        {"HTTP_QUERY_STRING", "currency_pair="+symbolName+"&limit=20"},
    });
    session.sendRequest(request);
  }

  std::this_thread::sleep_for(std::chrono::seconds(60));
  session.stop();
  std::cout << "Bye" << std::endl;
  return EXIT_SUCCESS;
}
