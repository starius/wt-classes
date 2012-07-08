
#include <cmath>
#include <stdexcept>
#include <fstream>
#include <boost/program_options/variables_map.hpp>

#include <Wt/WApplication>
#include <Wt/WText>
#include <Wt/WLineEdit>
#include <Wt/Wc/Wbi.hpp>
#include <Wt/Wc/util.hpp>

using namespace Wt;
using namespace Wt::Wc;

namespace po = boost::program_options;

void solve_quadratic(const po::variables_map& vm) {
    int a = vm["a"].as<int>();
    int b = vm["b"].as<int>();
    int c = vm["c"].as<int>();
    std::string out_filename = vm["o"].as<std::string>();
    std::ofstream out(out_filename.c_str());
    if (a == 0) {
        throw std::invalid_argument("a = 0");
    }
    int D = b * b - 4 * a * c;
    float two_a = 2 * a;
    float extreme_point = float(-b) / two_a;
    if (D < 0) {
        throw std::domain_error("D < 0");
    } else if (D == 0) {
        out << "x = " << extreme_point;
    } else { /* D > 0 */
        float sqrt_D = std::sqrt(D);
        out << "x1 = " << extreme_point - sqrt_D / std::abs(two_a);
        out << ", ";
        out << "x2 = " << extreme_point + sqrt_D / std::abs(two_a);
    }
}

class QuadraticEquationApp : public WApplication {
public:
    QuadraticEquationApp(const WEnvironment& env):
        WApplication(env) {
        enableUpdates(true);
        messageResourceBundle().use(approot() + "locales/wtclasses");
        new WText("Solve a quadratic equation", root());
        TableTask* task = new TableTask(root());
        task->add_input(new LineEditInput(new WLineEdit("1"), "--a"), "x2 (a)");
        desc_.add_options()("a", po::value<int>());
        task->add_input(new LineEditInput(new WLineEdit("-2"), "--b"), "x (b)");
        desc_.add_options()("b", po::value<int>());
        task->add_input(new LineEditInput(new WLineEdit("1"), "--c"), "1 (c)");
        desc_.add_options()("c", po::value<int>());
        task->add_output(new PrintFileOutput("--o"), "Roots of equation");
        desc_.add_options()("o", po::value<std::string>());
        task->set_runner(new BoostOptionsRunner(solve_quadratic, &desc_));
    }

private:
    po::options_description desc_;
};

WApplication* createQuadraticEquationApp(const WEnvironment& env) {
    return new QuadraticEquationApp(env);
}

int main(int argc, char** argv) {
    return WRun(argc, argv, &createQuadraticEquationApp);
}

