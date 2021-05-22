#include "pch.h"
#include "magic_squares.h"


using json = nlohmann::json;


std::unordered_set<int> __init_domain(int order)
{
	std::unordered_set<int> domain;
	domain.reserve(order);
	for (int i = 1; i <= order; ++i)
	{
		domain.emplace(i);
	}
	return domain;
}

void __init_name_and_square_to_var_ref_map(int order, const std::unordered_set<int>& domain,
	std::vector<csp::Variable<int>>& variables,
	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>>& squareToVarRefMap,
	std::unordered_map<std::string, std::reference_wrapper<csp::Variable<int>>>& nameToVarRefMap)
{
	variables.reserve(order);
	for (int i = 1; i <= order; ++i)
	{
		variables.emplace_back(domain);
		squareToVarRefMap.emplace(i, variables.back());
		nameToVarRefMap.emplace(std::to_string(i), variables.back());
	}
}

void __emplace_all_diff_constraint(int order, std::vector<csp::Variable<int>>& variables,
	std::vector<csp::Constraint<int>>& constraints)
{
	std::vector<std::reference_wrapper<csp::Variable<int>>> varRefs;
	varRefs.reserve(order);
	for (csp::Variable<int>& var : variables)
	{
		varRefs.emplace_back(var);
	}

	constraints.emplace_back(varRefs, csp::allDiff<int>);
}

void __emplace_row_constraints(int n, int order,
	csp::ExactLengthExactSum<int>& exactMagicSumConstrEvaluator,
	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>>& squareToVarRefMap,
	std::vector<csp::Constraint<int>>& constraints)
{
	for (int row = 1; row <= order; row += n)
	{
		std::vector<std::reference_wrapper<csp::Variable<int>>> varRefs;
		varRefs.reserve(n);
		for (int i = row; i < row + n; ++i)
		{
			varRefs.emplace_back(squareToVarRefMap.at(i));
		}
		constraints.emplace_back(varRefs, exactMagicSumConstrEvaluator);
	}
}

void __emplace_column_constraints(int n, int order,
	csp::ExactLengthExactSum<int>& exactMagicSumConstrEvaluator,
	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>>& squareToVarRefMap,
	std::vector<csp::Constraint<int>>& constraints)
{
	for (int col = 1; col <= n; ++col)
	{
		std::vector<std::reference_wrapper<csp::Variable<int>>> varRefs;
		varRefs.reserve(n);
		for (int i = col; i <= order; i += n)
		{
			varRefs.emplace_back(squareToVarRefMap.at(i));
		}
		constraints.emplace_back(varRefs, exactMagicSumConstrEvaluator);
	}
}

void __emplace_diagonal_constraints(int n, int order,
	csp::ExactLengthExactSum<int>& exactMagicSumConstrEvaluator,
	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>>& squareToVarRefMap,
	std::vector<csp::Constraint<int>>& constraints)
{
	std::vector<std::reference_wrapper<csp::Variable<int>>> firstDiagVarRefs;
	for (int i = 1; i <= order; i += n + 1)
	{
		firstDiagVarRefs.emplace_back(squareToVarRefMap.at(i));
	}
	constraints.emplace_back(firstDiagVarRefs, exactMagicSumConstrEvaluator);

	std::vector<std::reference_wrapper<csp::Variable<int>>> secondDiagVarRefs;
	for (int i = n; i < order; i += n - 1)
	{
		secondDiagVarRefs.emplace_back(squareToVarRefMap.at(i));
	}
	constraints.emplace_back(secondDiagVarRefs, exactMagicSumConstrEvaluator);
}

csp::ConstraintProblem<int> constructMagicSquareProblem(int n, std::vector<csp::Variable<int>>& variables,
	std::vector<csp::Constraint<int>>& constraints)
{
	int order = n * n;
	int magicSum = n * ((order + 1) / 2);

	std::unordered_set<int> domain = __init_domain(order);

	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>> squareToVarRefMap;
	std::unordered_map<std::string, std::reference_wrapper<csp::Variable<int>>> nameToVarRefMap;
	__init_name_and_square_to_var_ref_map(order, domain, variables, squareToVarRefMap, nameToVarRefMap);

	constraints.reserve(order);
	__emplace_all_diff_constraint(order, variables, constraints);

	csp::ExactLengthExactSum<int> exactMagicSumConstrEvaluator{ static_cast<unsigned int>(n), magicSum };

	__emplace_row_constraints(n, order, exactMagicSumConstrEvaluator, squareToVarRefMap, constraints);
	__emplace_column_constraints(n, order, exactMagicSumConstrEvaluator, squareToVarRefMap, constraints);
	__emplace_diagonal_constraints(n, order, exactMagicSumConstrEvaluator, squareToVarRefMap, constraints);

	std::vector<std::reference_wrapper<csp::Constraint<int>>> constraintsRefs{ constraints.begin(), constraints.end() };
	csp::ConstraintProblem<int> magicSquareProblem{ constraintsRefs, nameToVarRefMap };
	return magicSquareProblem;
}

json get_magic_squares_solutions(int n, const std::string& strNSquared)
{
	json solutions;
	solutions[strNSquared] = { };

	std::vector<csp::Variable<int>> magicSquareVars;
	std::vector<csp::Constraint<int>> magicSquareConstrs;
	csp::ConstraintProblem<int> magicSquareProb =
		constructMagicSquareProblem(n, magicSquareVars, magicSquareConstrs);

	const std::unordered_set<csp::Assignment<int>> sols =
		csp::heuristicBacktrackingSolver_findAllSolutions<int>(
			magicSquareProb,
			csp::minimumRemainingValues_primarySelector<int>,
			csp::degreeHeuristic_secondarySelector<int>,
			csp::leastConstrainingValue<int>);

	std::clog << "magic squares found " << sols.size() << " solutions\n";

	for (size_t i = 0; i < sols.size(); ++i)
		solutions[strNSquared].push_back({ });

	size_t i = 0;
	for (const csp::Assignment<int>& assignment : sols)
	{
		magicSquareProb.unassignAllVariables();
		magicSquareProb.assignFromAssignment(assignment);
		std::unordered_map<std::string, int> nameToSol = magicSquareProb.GetNameToSolution();
		json j = nameToSol;
		solutions[strNSquared][i] = j;
		++i;
	}

	std::clog << "magic squares sends: \n";
	std::clog << solutions;
	std::clog << '\n';

	return solutions;
}