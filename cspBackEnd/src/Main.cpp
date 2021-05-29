#include "pch.h"
#include "custom_logger.h"
#include "pythagorean_triples.h"
#include "magic_squares.h"
#include "n_queens.h"


using json = nlohmann::json;


std::unordered_map<int, json> g_pythSolutions;
std::unordered_map<int, json> g_magicSquareSolutions;
std::unordered_map<int, json> g_nQueensSolutions;


// currently only supports hueBacktracking
const char* ALGORITHMS =
"{algo: backtracking|hueBacktracking|hueBacktrackingForwardChecking|hueBacktrackingMAC|"
"minConflicts|constraintsWeighting|simulatedAnnealing|randomRestartFirstChoiceHillClimbing|geneticLocalSearch}";


void pythagorean_triples_handler(const std::shared_ptr< restbed::Session > session)
{
    const auto& request = session->get_request();
    std::string upperBound = request->get_path_parameter("upperBound");
    int n = stoi(upperBound);
    std::string algo = request->get_path_parameter("algo");

    std::clog << "pythagorean triples got upperBound: " << upperBound << '\n';
    std::clog << "pythagorean triples got algo: " << algo << '\n';

    std::string res = "";

    if (g_pythSolutions.count(n))
    {
        std::clog << "pythagorean triples sends cached solution\n";
        res = g_pythSolutions[n].dump();
    }
    else
    {
        json solutions = get_pythagorean_triples_solutions(upperBound);
        g_pythSolutions[n] = solutions;
        res = solutions.dump();
    }

    session->close(restbed::OK, res, { { "Content-Length", std::to_string(res.size()) } });
}

void magic_square_handler(const std::shared_ptr< restbed::Session > session)
{
    const auto& request = session->get_request();
    std::string strMagicSquareSize = request->get_path_parameter("magicSquareSize");
    int magicSquareSize = std::stoi(strMagicSquareSize);
    std::string algo = request->get_path_parameter("algo");

    std::clog << "magic squares got magicSquareSize: " << strMagicSquareSize << '\n';
    std::clog << "magic squares got algo: " << algo << '\n';

    std::string res = "";

    if (g_magicSquareSolutions.count(magicSquareSize))
    {
        std::clog << "magic squares sends cached solution\n";
        res = g_magicSquareSolutions[magicSquareSize].dump();
    }
    else
    {
        json solutions = get_magic_squares_solutions(magicSquareSize, strMagicSquareSize);
        g_magicSquareSolutions[magicSquareSize] = solutions;
        res = solutions.dump();
    }

    session->close(restbed::OK, res, { { "Content-Length", std::to_string(res.size()) } });
}

void n_queens_handler(const std::shared_ptr< restbed::Session > session)
{
    const auto& request = session->get_request();
    std::string strNQueensSize = request->get_path_parameter("nQueensSize");
    int n = stoi(strNQueensSize);
    std::string algo = request->get_path_parameter("algo");
    std::clog << "n queens got n: " << strNQueensSize << '\n';
    std::clog << "n queens got algo: " << algo << '\n';

    std::string res = "";

    if (g_nQueensSolutions.count(n))
    {
        std::clog << "n queens sends cached solution\n";
        res = g_nQueensSolutions[n].dump();
    }
    else
    {
        json solutions = get_n_queens_solutions(strNQueensSize);
        g_nQueensSolutions[n] = solutions;
        res = solutions.dump();
    }

    session->close(restbed::OK, res, { { "Content-Length", std::to_string(res.size()) } });
}

int main(const int argc, const char** argv)
{
    auto pythResource = std::make_shared< restbed::Resource >();
    std::string pythPath = "/resources/pythagorianSolutions/{upperBound: [1-9][0-9]*}/";
    pythPath += ALGORITHMS;
    pythResource->set_path(pythPath);
    pythResource->set_method_handler("GET", pythagorean_triples_handler);

    auto magicSquareResource = std::make_shared< restbed::Resource >();
    std::string magicSquarePath = "/resources/magicSquareSolutions/{magicSquareSize: [1-9][0-9]*}/";
    magicSquarePath += ALGORITHMS;
    magicSquareResource->set_path(magicSquarePath);
    magicSquareResource->set_method_handler("GET", magic_square_handler);

    auto nQueensResource = std::make_shared< restbed::Resource >();
    std::string nQueensPath = "/resources/nQueensSolutions/{nQueensSize: [1-9][0-9]*}/";
    nQueensPath += ALGORITHMS;
    nQueensResource->set_path(nQueensPath);
    nQueensResource->set_method_handler("GET", n_queens_handler);

    auto settings = std::make_shared< restbed::Settings >();
    settings->set_worker_limit(4);
    settings->set_port(8080);
    settings->set_default_header("Connection", "close");
    settings->set_default_header("Access-Control-Allow-Origin", "*");

    restbed::Service service;
    service.publish(pythResource);
    service.publish(magicSquareResource);
    service.publish(nQueensResource);
    service.set_logger(std::make_shared< CustomLogger >());

    service.start(settings);

    return EXIT_SUCCESS;
}