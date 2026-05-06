#include "GlobalController/IED/IED.h"
#include "include.h"

#include <future>
#include <mutex>

#define POS_STR_VAL    700.0   // уставка по приращению модуля PositiveSeq, А
#define POS_STR_ANG    0.5     // уставка по приращению угла PositiveSeq, рад
#define POS_TIME_S     0.6    // выдержка PIOC1, c
#define NEG_STR_VAL    700.0    // уставка по приращению модуля NegativeSeq, А
#define NEG_STR_ANG    0.5     // уставка по приращению угла NegativeSeq, рад
#define NEG_TIME_S     0.25    // выдержка PIOC2, c
#define KMAN           4.0     // коэффициент в формуле iман
#define IMAN_THR       500.0   // уставка по току манипуляции
#define DISCRETIZATION 4000
#define FOURIER_MODE   true


int main(int argc, char* argv[])
{
    std::pair<std::string, std::string> comtrade1, comtrade2;

    if (argc < 3) {
        comtrade1.first  = "End_line/K3_1.cfg";
        comtrade1.second = "End_line/K3_1.dat";
        comtrade2.first  = "Start_line/K6_1.cfg";
        comtrade2.second = "Start_line/K6_1.dat";
    } else {
        comtrade1.first  = std::string("End_line/")   + argv[1] + ".cfg";
        comtrade1.second = std::string("End_line/")   + argv[1] + ".dat";
        comtrade2.first  = std::string("Start_line/") + argv[2] + ".cfg";
        comtrade2.second = std::string("Start_line/") + argv[2] + ".dat";
    }

    const std::string name1 = "DPP_1";
    const std::string name2 = "DPP_2";

    auto ied1 = std::make_shared<IED>(name1);
    auto ied2 = std::make_shared<IED>(name2);

    try {
        auto parser1 = std::make_shared<ParserComtrade>(comtrade1);
        auto parser2 = std::make_shared<ParserComtrade>(comtrade2);

        int times = parser1->getTimeDataSize();
        if (times != parser2->getTimeDataSize()) {
            std::__throw_length_error("Длины выборок, записанных внутри переданных файлов, расходятся");
        }

        auto context = std::make_shared<zmq::context_t>();

        ied1->setSettings(std::make_shared<Settings>(
            POS_STR_VAL, POS_STR_ANG, POS_TIME_S,
            NEG_STR_VAL, NEG_STR_ANG, NEG_TIME_S,
            KMAN, IMAN_THR, FOURIER_MODE, DISCRETIZATION,
            parser1, context, name2));
        ied2->setSettings(std::make_shared<Settings>(
            POS_STR_VAL, POS_STR_ANG, POS_TIME_S,
            NEG_STR_VAL, NEG_STR_ANG, NEG_TIME_S,
            KMAN, IMAN_THR, FOURIER_MODE, DISCRETIZATION,
            parser2, context, name1));

        py::scoped_interpreter guard{};

        intptr_t ctx_ptr = reinterpret_cast<intptr_t>(static_cast<void*>(context->operator void*()));

        /* PULL on telemetry must bind before IED PUSH connects (inproc ordering). */
        std::promise<void> telemetry_pull_bound;
        std::future<void> telemetry_pull_bound_f = telemetry_pull_bound.get_future();

        std::vector<std::thread> protThreads;
        {
            py::gil_scoped_release release;

            protThreads.emplace_back(run_python_script, ctx_ptr, "Monitoring.py", &telemetry_pull_bound);
            telemetry_pull_bound_f.wait();

            
            

            for (auto& ied : {ied1, ied2}) {
                ied->bindPSCHLocal();
                ied->connectPSCHRemote();
                ied->IEDInitDataTransfer();
                protThreads.emplace_back(&IED::modelIEDWork, ied.get(), std::ref(times));
            }

            for (auto& thread : protThreads) {
                thread.join();
            }
        }

    } catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка преобразования: " << e.what() << std::endl;
    } catch (const std::length_error& e) {
        std::cerr << "Ошибка в длине: " << e.what() << std::endl;
    }

    return 0;
}

void run_python_script(intptr_t ctx_address, const std::string& script_path, std::promise<void>* telemetry_pull_ready)
{
    py::gil_scoped_acquire gil;
    py::globals()["zmq_context_address"] = ctx_address;
    if (telemetry_pull_ready) {
        py::globals()["notify_telemetry_pull_bound"] = py::cpp_function([telemetry_pull_ready]() {
            static std::once_flag once;
            std::call_once(once, [&] { telemetry_pull_ready->set_value(); });
        });
    }
    py::eval_file(script_path);
}
