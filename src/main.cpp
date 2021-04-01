#include <mukyu/cablin/config/yamlconfig.hpp>
#include <mukyu/cablin/core/script.hpp>
#include <mukyu/cablin/core/value.hpp>
#include <mukyu/cablin/core/controller.hpp>
#include <mukyu/cablin/core/function.hpp>
#include <mukyu/cablin/function/functor.hpp>

#include <emscripten/emscripten.h>

#include <iostream>
#include <exception>

namespace mccore = mukyu::cablin::core;
namespace mcconf = mukyu::cablin::config;
namespace mcfunc = mukyu::cablin::function;


const std::string MYPACKAGE_NAME = "my";

class MyPackage : public mccore::Package {
public:
    std::function<void(const char*)> printFunc_;

    MyPackage(std::function<void(const char*)> printFunc): printFunc_(printFunc) {}
    ~MyPackage() = default;

    void prepare(mccore::Controller* controller) {
        controller->addPackage(MYPACKAGE_NAME);

        auto printFunc = [this](mccore::ValueList params) {
            if (params.size() != 1) {
                throw std::runtime_error("print params should be 1");
            }
            const auto& param = params[0];

            std::string msg;

            switch (param.type()) {
            case mccore::ValueType::STRING:
                msg = param.as<std::string>();
                break;
            case mccore::ValueType::BOOL:
                msg = param.as<bool>() ? "true" : "false";
                break;
            case mccore::ValueType::FLOAT:
                msg = std::to_string(param.as<float>());
                break;
            case mccore::ValueType::INT:
                msg = std::to_string(param.as<int>());
                break;
            case mccore::ValueType::INT64:
                msg = std::to_string(param.as<int64_t>());
                break;
            default:
                msg = "null";
                break;
            }

            printFunc_(msg.c_str());

            return 0;
        };
        controller->addFunction(
            MYPACKAGE_NAME,
            std::make_shared<mcfunc::FunctionFunctor>("print", printFunc));
    }

    std::string name() const {
        return MYPACKAGE_NAME;
    }
};


std::string cablinError;


extern "C" {


const char* getCablinError() {
    return cablinError.c_str();
}

int EMSCRIPTEN_KEEPALIVE cablinRun(const char* body, void(*print)(const char*)) {
    try {
        std::string strBody(body);
        auto conf = mcconf::createYAMLConfigFromString(body);
        mccore::Script script(".");
        script.addPackage(std::make_shared<MyPackage>(print));
        script.addYamlNode("main", conf);
        script.callFunction("main", "main", {});
    } catch (const std::runtime_error& re) {
        std::cout << "cablinRun: runtime error:" << re.what() << std::endl;
        cablinError = re.what();
        return 1;
    } catch (const std::exception& ex) {
        std::cout << "cablinRun: " << ex.what() << std::endl;
        cablinError = ex.what();
        return 1;
    } catch (...) {
        std::cout << "cablinRun: unknown error" << std::endl;
        cablinError = "unknown error";
        return 1;
    }

    cablinError = "";
    return 0;
}


}
