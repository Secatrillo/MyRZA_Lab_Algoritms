#include "GlobalController/IED/IED.h"
#include "include.h"

#include <cmath>
#include <future>

#define POS_STR_VAL    200.0
#define POS_STR_ANG    0.5
#define NEG_STR_VAL    70.0
#define NEG_STR_ANG    0.5
#define DISCRETIZATION 4000
#define FOURIER_MODE   true

/* PDIS1 — уставки зоны срабатывания (OpDlTmms в с; StrValFi в градусах → рад в коде). */
#define PDIS1_OP_DL_TMMS      0.5
#define PDIS1_STR_VAL_R       10
#define PDIS1_STR_VAL_X       10
#define PDIS1_STR_VAL_FI_DEG  0.0
#define PDIS1_STR_VAL_OFFSET  0.0
#define PDIS1_STR_VAL_HYST    1.05

/* PDIS2 */
#define PDIS2_OP_DL_TMMS      0.8
#define PDIS2_STR_VAL_R       50
#define PDIS2_STR_VAL_X       20
#define PDIS2_STR_VAL_FI_DEG  45
#define PDIS2_STR_VAL_OFFSET  45
#define PDIS2_STR_VAL_HYST    1.05

/* PDIS3 */
#define PDIS3_OP_DL_TMMS      1.0
#define PDIS3_STR_VAL_R       75
#define PDIS3_STR_VAL_X       18
#define PDIS3_STR_VAL_FI_DEG  55
#define PDIS3_STR_VAL_OFFSET  75
#define PDIS3_STR_VAL_HYST    1.05

static std::pair<std::string, std::string> dprotPair(const std::string& stem)
{
    return {"DProtComt/" + stem + ".cfg", "DProtComt/" + stem + ".dat"};
}

int main(int argc, char* argv[])
{
    std::string stemCurrent = "K3_Q4-BCG";
    std::string stemVoltage = "K3U_Q4-BCG";

    if (argc >= 3) {
        stemCurrent = argv[1];
        stemVoltage = argv[2];
    }

    const std::string iedName = "DPP_1";
    IED ied(iedName);

    try {
        auto parserCurrent = std::make_shared<ParserComtrade>(dprotPair(stemCurrent));
        auto parserVoltage = std::make_shared<ParserComtrade>(dprotPair(stemVoltage));

        int times = parserCurrent->getTimeDataSize();
        if (times != parserVoltage->getTimeDataSize()) {
            std::__throw_length_error("Длины выборок COMTRADE токов и напряжений должны совпадать");
        }

        auto context = std::make_shared<zmq::context_t>();

        const PdisPickupSettings pdis1{PDIS1_OP_DL_TMMS,
                                       PDIS1_STR_VAL_R,
                                       PDIS1_STR_VAL_X,
                                       (PDIS1_STR_VAL_FI_DEG) * (M_PI / 180.0),
                                       PDIS1_STR_VAL_OFFSET,
                                       PDIS1_STR_VAL_HYST};
        const PdisPickupSettings pdis2{PDIS2_OP_DL_TMMS,
                                       PDIS2_STR_VAL_R,
                                       PDIS2_STR_VAL_X,
                                       (PDIS2_STR_VAL_FI_DEG) * (M_PI / 180.0),
                                       PDIS2_STR_VAL_OFFSET,
                                       PDIS2_STR_VAL_HYST};
        const PdisPickupSettings pdis3{PDIS3_OP_DL_TMMS,
                                       PDIS3_STR_VAL_R,
                                       PDIS3_STR_VAL_X,
                                       (PDIS3_STR_VAL_FI_DEG) * (M_PI / 180.0),
                                       PDIS3_STR_VAL_OFFSET,
                                       PDIS3_STR_VAL_HYST};

        ied.setSettings(std::make_shared<Settings>(
            POS_STR_VAL, POS_STR_ANG,
            NEG_STR_VAL, NEG_STR_ANG,
            FOURIER_MODE, DISCRETIZATION,
            parserCurrent, parserVoltage, context,
            pdis1, pdis2, pdis3));

        py::scoped_interpreter guard{};

        intptr_t ctx_ptr = reinterpret_cast<intptr_t>(static_cast<void*>(context->operator void*()));

        std::promise<void> telemetry_pull_bound;
        std::future<void> telemetry_pull_bound_f = telemetry_pull_bound.get_future();

        // Matplotlib/Qt ожидают GUI в потоке, где создан интерпретатор — запускаем Monitoring здесь,
        // симуляцию C++ — в фоне. gil_scoped_release здесь нельзя: поток не держал GIL (PyThreadState NULL).
        std::thread simThread([&]() {
            telemetry_pull_bound_f.wait();
            ied.IEDInitDataTransfer();
            ied.modelIEDWork(times);
        });

        {
            py::gil_scoped_acquire gil;
            py::globals()["zmq_context_address"] = ctx_ptr;
            py::globals()["notify_telemetry_pull_bound"] = py::cpp_function([&telemetry_pull_bound]() {
                static std::once_flag once;
                std::call_once(once, [&] { telemetry_pull_bound.set_value(); });
            });
            py::eval_file("Monitoring.py");
        }

        simThread.join();

    } catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка преобразования: " << e.what() << std::endl;
    } catch (const std::length_error& e) {
        std::cerr << "Ошибка в длине: " << e.what() << std::endl;
    }

    return 0;
}
