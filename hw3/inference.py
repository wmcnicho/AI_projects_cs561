import sys
import re

queries = []
clauses = []
KB = {}

def run_inference(KB, query):
    #print("Query is " + query)
    stack = []
    match = re.search(r"~?[A-Z]+[a-zA-Z]*\(", query)
    query_predicate = match.string[match.start():match.end()-1]
    if query_predicate not in KB:
        KB[query_predicate] = PredicateEntry()
    return bc_or(KB, query, stack)

def unify(a, b):#returns a subsitution map
    if( (not isinstance(a, Literal) ) or ( not isinstance(b, Literal) )):
        print("unify used in unexpected way")
        return None
    #if the predicates are not equal, return failure
    if(a.predicate != b.predicate):
        return None
    subsitution = {}
    list_one = a.arguments
    list_two = b.arguments
    if(len(list_one) != len(list_two)):
        return None
    for index in range(0, len(list_one)):
        #if equal move on
        if(list_one[index] == list_two[index]):
            continue
        lhs_is_var = re.match(r'^([a-z])', list_one[index])
        rhs_is_var = re.match(r'^([a-z])', list_two[index])
        if(lhs_is_var and rhs_is_var):
            #print("trying to unify two variables, don't do anything?")
            do_nothing = ""
        #if one variable and one constant, subsitute Constant for variable
        elif(lhs_is_var):
            subsitution[lhs_is_var.string[lhs_is_var.start():lhs_is_var.end()]] = list_two[index];
        elif(rhs_is_var):
            #subsitution from rhs this should not occur as it makes the statement less general
            do_nothing = ""
            #subsitution[rhs_is_var.string[rhs_is_var.start():rhs_is_var.end()]] = list_one[index];
        else:#they're both constants
            if(list_one[index] != list_two[index]):
                return None#subsitution failed because Constant mismatch
    return subsitution

def bc_or(KB, goal, stack):
    #print(goal)
    loop_test = Literal(goal)
    if(loop_test.getVarCount() == 0):
        if goal in stack:
            #Loop detected
            #print('Loop! Skipping')
            return False
    stack.append(goal)
    if not isinstance(goal, str):
        print("bc_or used in unexpected way")
    match = re.search(r"~?[A-Z]+[a-zA-Z]*\(", goal)
    predicate = match.string[match.start():match.end()-1]

    possible_solutions = KB[predicate].literals
    for solution in possible_solutions:
        if(goal == solution):
            stack.pop()
            return True

    satisfied = False
    possible_choices = KB[predicate].conclusions
    for possibility in possible_choices:
        expression = Expression(possibility)
        substitution = unify(expression.conclusion, Literal(goal))
        #TODO would I ever need to subsitute the rhs, if so i need to standardize the unification
        if(substitution is not None):
            #substitute
            expression.substitute_args(substitution)
            array = expression.getPremisesList()
            if(bc_and(KB, expression.getPremisesList(), stack)):
                stack.pop()
                return True
    stack.pop()
    return satisfied

def bc_and(KB, goal, stack):
    #if goal is empty return True
    if(goal == []):
        print("Goal is empty")
        return True

    #test that all of the goal conditions can be satisfied with any possible subsitution


   #for each possible literal solution for the first goal
        #unify
        #if unify sucessful, do subsitute for all goals and test if they are in the KB bc_or
    all_clauses_true = True
    for clause in goal:
        match = re.search(r"~?[A-Z]+[a-zA-Z]*\(", clause)
        predicate = match.string[match.start():match.end()-1]

        clause_true = False
        possible_solutions = KB[predicate].literals
        for fact in possible_solutions:
            substitution = unify(Literal(clause), Literal(fact))
            valid_substitution = True
            if(substitution is not None):
                #substitute for every literal and check if solution fails because of subsitution
                for other_clause in goal:
                    lit_clause = Literal(other_clause)
                    affected = lit_clause.has_var_in_map(substitution)
                    if(affected):
                        lit_clause.substitute_from_map(substitution)
                        other_clause_true = bc_or(KB, str(lit_clause), stack)
                        valid_substitution = valid_substitution and other_clause_true
                if(valid_substitution):
                    clause_true = True
        if(not clause_true):
            clause_true = bc_or(KB, clause, stack)
        if(not clause_true):
            return clause_true
    return  all_clauses_true

    # all_satisfied =  True
    # for premise in goal:
    #     is_true = bc_or(KB, premise, stack)
    #     all_satisfied = all_satisfied and is_true
    # return all_satisfied

class PredicateEntry:
    literals = []
    conclusions = []
    premises = []
    def __init__(self):
        self.literals = []
        self.conclusions = []
        self.premises = []
    def __repr__(self):
        return "facts: %s, conclusions: %s, premises: %s" % (self.literals, self.conclusions, self.premises)

class Literal:
    def has_var_in_map(self, sub_map):
        for sub in sub_map:
            if sub in self.arguments:
                return True
        return False
    def substitute_from_map(self, sub_map):
        for sub in sub_map:
            self.substitute(sub, sub_map[sub])
    def substitute(self, var_name, value):
        for index in range(0, len(self.arguments)):
            if(self.arguments[index] == var_name):
                self.arguments[index] = value;
    def getVarCount(self):
        count = 0;
        for arg in self.arguments:
            match = re.match(r'^([a-z])', arg)
            if(match):
                count = count + 1
        return count
    def __init__(self):
        self.predicate = ""
        self.arguments = []
        self.root_string = ""
        self.positive = True
        self.subs = {}
    def __init__(self, input_string):
        self.root_string = input_string
        self.subs = {}
        match = re.search(r"[A-Z]+[a-zA-Z]*\(", input_string)
        self.predicate = match.string[match.start():match.end()-1]
        self.positive = True
        if(input_string[0] == '~'):
            self.positive = False
        inside_paren = re.search(r"\(([A-Za-z0-9,]+)\)", input_string).group(1)
        self.arguments = re.split(",", inside_paren)
    def __repr__(self):
        arg_string = ""
        for arg in self.arguments:
            arg_string += arg + ","
            match = re.match(r'^([a-z])', arg)
            if(match):
                self.subs[match.string[match.start():match.end()]] = None
        arg_string = arg_string.rstrip(',')
        if(self.positive):
            return self.predicate + "(" + arg_string + ")"
        else:
            return "~" + self.predicate + "(" + arg_string + ")"

class Expression:
    def substitute_args(self, sub_map):
        for variable in sub_map:
            for literal in self.premises:
                literal.substitute(variable, sub_map[variable])
            self.conclusion.substitute(variable, sub_map[variable])
    def getPremisesList(self):
        arr = []
        for premise in self.premises:
            arr.append(str(premise))
        return arr
    def __init__(self, input_string):
        self.root_string = input_string
        sides = re.split(r' => ', input_string)
        lhs = sides[0]
        rhs = sides[1]
        self.conclusion = Literal(rhs)
        self.premises = []
        lhs_literals = re.split(r' \^ ', lhs)
        #for each predicate in lhs
        for literal in lhs_literals:
            self.premises.append(Literal(literal))
    def __repr__(self):
        output = ""
        for premise in self.premises:
            output += str(premise) + " ^ "
        output = output.rstrip(" ^ ")
        output += " => " + str(self.conclusion)
        return output

#File Input
file = open(sys.argv[2], 'r')
num_queries = int(file.readline().rstrip('\r\n'))
for num in range(0, num_queries):
    line = file.readline().rstrip('\r\n')
    queries.append(line)
num_clauses = int(file.readline().rstrip('\r\n'))
for num in range(0, num_clauses):
    line = file.readline().rstrip('\r\n')
    clauses.append(line)
file.close()

#Create table-indexed KB
#iterate over each clause
for clause in clauses:
    #print(clause)
    #determine if it is a fact or an expression (regex check for =>)
    expression = re.search(r' => ', clause)
    #if fact
    if expression is None:
        #check if true or false
        #print("It's a fact")
        #parse the predicate
        match = re.search(r"~?[A-Z]+[a-zA-Z]*\(", clause)
        predicate = match.string[match.start():match.end()-1]
        #print("Predicate is " + predicate)
        first_char = clause[0]
        if predicate not in KB:
            KB[predicate] = PredicateEntry()
        KB[predicate].literals.append(clause)
    #elif expression
    elif expression is not None:
        #print("It's an expression")
        #split lhs and rhs
        sides = re.split(r' => ', clause)
        lhs = sides[0]
        rhs = sides[1]
        #print("LHS is: " + lhs + " RHS is: " + rhs)
        #split lhs by ^
        lhs_literals = re.split(r' \^ ', lhs)
        #for each predicate in lhs
        for literal in lhs_literals:
            match = re.search(r"~?[A-Z]+[a-zA-Z]*\(", literal)
            predicate = match.string[match.start():match.end()-1]
            if predicate not in KB:
                KB[predicate] = PredicateEntry()
            #add string to premise entry
            KB[predicate].premises.append(clause)
        match = re.search(r"~?[A-Z]+[a-zA-Z]*\(", rhs)
        predicate = match.string[match.start():match.end()-1]
        if predicate not in KB:
            KB[predicate] = PredicateEntry()
        #add rhs to conclusion entry of predicate
        KB[predicate].conclusions.append(clause)

#for knowledge in KB:
    #print(knowledge + ": " + str(KB[knowledge]))

#Test queries (File Output)
output = open('output.txt', 'w')
for query in queries:
    #run a backward chaining unification algorithm to test each query
    val = run_inference(KB, query)
    if(val):
        output.write("TRUE\n")
        print(query + ": TRUE")
    else:
        output.write("FALSE\n")
        print(query + ": FALSE")
output.close()