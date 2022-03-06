#include "formula.h"
#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>
#include <utility>

using namespace std;

FormulaError::FormulaError(Category category) 
	: category_(category) {
}

FormulaError::Category FormulaError::GetCategory() const {
	return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
	return category_ == rhs.category_;
}

string_view FormulaError::ToString() const {
	switch (category_) {
		case Category::Ref:
			return "#REF!";
		case Category::Value:
			return "#VALUE!";
		case Category::Div0:
			return "#DIV/0!";
	}
	return "";
}

double GetDoubleFrom(const string &str) {
	double value = 0;
	if (!str.empty()) {
		istringstream in(str);
		
		if (!(in >> value) || !in.eof()) {
			throw FormulaError(FormulaError::Category::Value);
		}
	}
	return value;
}

double GetDoubleFrom(const double value) {
	return value;
}

double GetDoubleFrom(const FormulaError error) {
	throw FormulaError(error);
}

double GetCellValue(const CellInterface *cell) {
	if (!cell) {
		return 0;
	} else {
		return visit([](const auto &value) { return GetDoubleFrom(value); }, cell->GetValue());
	}
}

ostream& operator<<(ostream& output, FormulaError fe) {
	return output << fe.ToString();
}

namespace {
	class Formula : public FormulaInterface {
	public:
		explicit Formula(string expression)
		try
			: ast_(ParseFormulaAST(expression)) {
		} catch (const exception &exc) {
			throw_with_nested(FormulaException(exc.what()));
		}

		Value Evaluate(const SheetInterface& sheet) const override {
			try {
				ExecuteCell execute_cell = [&sheet](Position pos) {
					if (!pos.IsValid()) {
						throw FormulaError(FormulaError::Category::Ref);
					}

					const auto cell = sheet.GetCell(pos);

					return GetCellValue(cell);
				};

				return ast_.Execute(execute_cell);
			} catch (FormulaError& error) {
				return error;
			}
		}

		string GetExpression() const override {
			ostringstream stream;
			ast_.PrintFormula(stream);

			return stream.str();
		}

		vector<Position> GetReferencedCells() const override {
			vector<Position> cells;
			for (auto cell : ast_.GetCells()) {
				if (cell.IsValid()) {
					cells.push_back(cell);
				}
			}

			cells.resize(unique(cells.begin(), cells.end()) - cells.begin());
			
			return cells;
		}

	private:
		FormulaAST ast_;
	};
}  // namespace

unique_ptr<FormulaInterface> ParseFormula(string expression) {
	return make_unique<Formula>(move(expression));
}