#pragma once

#include "constraint_problem.h"


namespace csp
{
	template <typename T>
	using StartStateGenarator = std::function<ConstraintProblem<T>(ConstraintProblem<T>& srcConstraintProblem,
		std::vector<Variable<T>>& destVars, std::vector<Constraint<T>>& destConstraints)>;

	template <typename T>
	ConstraintProblem<T> generateStartStateRandomly(ConstraintProblem<T>& srcConstraintProblem,
		std::vector<Variable<T>>& destVars, std::vector<Constraint<T>>& destConstraints)
	{
		ConstraintProblem<T> copiedConstraintProb = srcConstraintProblem.deepCopy(destVars, destConstraints);
		copiedConstraintProb.unassignAllVariables();
		copiedConstraintProb.assignVarsWithRandomValues();
		return copiedConstraintProb;
	}
}