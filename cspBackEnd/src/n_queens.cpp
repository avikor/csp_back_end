#include "pch.h"
#include "n_queens.h"


using json = nlohmann::json;


std::unordered_set<int> __initDomain(int n)
{
	std::unordered_set<int> domain;
	domain.reserve(n);
	for (int i = 0; i < n; ++i)
	{
		domain.emplace(i);
	}
	return domain;
}

void __init_name_and_col_to_var_ref_map(int n, const std::unordered_set<int>& domain,
	std::vector<csp::Variable<int>>& variables,
	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>>& columnToVarRefMap,
	std::unordered_map<std::string, std::reference_wrapper<csp::Variable<int>>>& nameToVarRefMap)
{
	variables.reserve(n);
	for (int i = 0; i < n; ++i)
	{
		variables.emplace_back(domain);
		columnToVarRefMap.emplace(i, variables.back());
		nameToVarRefMap.emplace(std::to_string(i), variables.back());
	}
}

std::unordered_map<int, csp::ConstraintEvaluator<int>> __init_notAttackingConstraintEvaluatorsMap(int n)
{
	std::unordered_map<int, csp::ConstraintEvaluator<int>> notAttackingConstrainEvaluatorsMap;
	for (int columnDifference = 1; columnDifference < n; ++columnDifference)
	{
		notAttackingConstrainEvaluatorsMap.emplace(columnDifference,
			[columnDifference](const std::vector<int>& assignedValues) -> bool
			{
				if (assignedValues.size() < 2)
				{
					return true;
				}
				int firstRow = assignedValues[0];
				int secondRow = assignedValues[1];
				return firstRow != secondRow && std::abs(firstRow - secondRow) != columnDifference;
			});
	}
	return notAttackingConstrainEvaluatorsMap;
}

void __init_constraints(int n, std::vector<csp::Constraint<int>>& constraints,
	const std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>>& columnToVarRefMap,
	std::unordered_map<int, csp::ConstraintEvaluator<int>>& notAttackingConstrainEvaluatorsMap)
{
	constraints.reserve(((n * n) + n) / 2);
	for (int firstCol = 0; firstCol < n; ++firstCol)
	{
		for (int secondCol = 0; secondCol < n; ++secondCol)
		{
			if (firstCol < secondCol)
			{
				std::vector<std::reference_wrapper<csp::Variable<int>>> varRefs
				{
					std::reference_wrapper<csp::Variable<int>>{ columnToVarRefMap.at(firstCol) },
					std::reference_wrapper<csp::Variable<int>>{ columnToVarRefMap.at(secondCol) }
				};
				constraints.emplace_back(varRefs,
					notAttackingConstrainEvaluatorsMap[std::abs(firstCol - secondCol)]);
			}
		}
	}
}

csp::ConstraintProblem<int> constructNQueensProblem(int n, std::vector<csp::Variable<int>>& variables,
	std::vector<csp::Constraint<int>>& constraints)
{
	std::unordered_set<int> domain = __initDomain(n);

	std::unordered_map<int, std::reference_wrapper<csp::Variable<int>>> columnToVarRefMap;
	std::unordered_map<std::string, std::reference_wrapper<csp::Variable<int>>> nameToVarRefMap;
	__init_name_and_col_to_var_ref_map(n, domain, variables, columnToVarRefMap, nameToVarRefMap);

	std::unordered_map<int, csp::ConstraintEvaluator<int>> notAttackingConstrainEvaluatorsMap = __init_notAttackingConstraintEvaluatorsMap(n);

	__init_constraints(n, constraints, columnToVarRefMap, notAttackingConstrainEvaluatorsMap);

	std::vector<std::reference_wrapper<csp::Constraint<int>>> constraintsRefs{ constraints.begin(), constraints.end() };
	csp::ConstraintProblem<int> nQueensProblem{ constraintsRefs, nameToVarRefMap };
	return nQueensProblem;
}

json get_n_queens_solutions(const std::string& strN)
{
	json solutions;
	solutions[strN] = { };

	std::vector<csp::Variable<int>> nQueensVars;
	std::vector<csp::Constraint<int>> nQueensConstrs;
	csp::ConstraintProblem<int> nQueensProb = constructNQueensProblem(std::stoi(strN), nQueensVars, nQueensConstrs);

	const std::unordered_set<csp::Assignment<int>> sols =
		csp::heuristicBacktrackingSolver_findAllSolutions<int>(
			nQueensProb,
			csp::minimumRemainingValues_primarySelector<int>,
			csp::degreeHeuristic_secondarySelector<int>,
			csp::leastConstrainingValue<int>);

	std::clog << "n queens found " << sols.size() << " solutions\n";

	for (size_t i = 0; i < sols.size(); ++i)
		solutions[strN].push_back({ });

	size_t i = 0;
	for (const csp::Assignment<int>& assignment : sols)
	{
		nQueensProb.unassignAllVariables();
		nQueensProb.assignFromAssignment(assignment);
		std::unordered_map<std::string, int> nameToSol = nQueensProb.GetNameToSolution();
		json j = nameToSol;
		solutions[strN][i] = j;
		++i;
	}

	std::clog << "n queens sends: \n";
	std::clog << solutions;
	std::clog << '\n';

	return solutions;
}