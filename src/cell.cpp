#include "../inc/cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>
#include <stack>
#include <sstream>

using namespace std;

size_t HashPosition::operator() (const Position& pos) const {
	size_t hash_row = i_hasher(pos.row);
	size_t hash_col = i_hasher(pos.col);

	return hash_row + 27 * hash_col;
}

Cell::Cell(Sheet& sheet, Position pos)
	: sheet_(sheet)
	, pos_(pos)
	, impl_(make_unique<EmptyImpl>()) {
}

Cell::~Cell() {
	impl_.reset();
}

void Cell::Set(string text) {
	unique_ptr<Impl> tmp_impl;
	if (text.empty()) {
		tmp_impl = make_unique<EmptyImpl>();
	} else if (text.front() == FORMULA_SIGN && text.size() > 1) {
		try {
			tmp_impl = make_unique<FormulaImpl>(text.substr(1), sheet_);
		} catch (...) {
			throw FormulaException("Sytnax error!"s);
		}
	} else {
		tmp_impl = make_unique<TextImpl>(text);
	}

	if (IsCircular(tmp_impl.get())) {
		throw CircularDependencyException("Circular dependency!"s);
	}

	impl_ = move(tmp_impl);

	UpdateReferences();
	InvalidCache();
	InvalidAllDependentCaches(incoming_references_);
}

void Cell::Clear() {
	impl_ = make_unique<EmptyImpl>();
}

Cell::Value Cell::GetValue() const {
	if (!cache_.has_value()) {
		cache_ = impl_->GetValue();
	}

	return cache_.value();
}

string Cell::GetText() const {
	return impl_->GetText();
}

vector<Position> Cell::GetReferencedCells() const {
	return impl_->GetReferencedCells();
}

bool Cell::IsReferenced() const {
	return !outgoing_references_.empty() || !incoming_references_.empty();
}

Cell* Cell::MakeCell(Position pos) const {
	Cell* cell = sheet_.GetCellByIndex(pos);
	if (!cell) {
		sheet_.SetCell(pos, string());
		cell = MakeCell(pos);
	}

	return cell;
}

bool Cell::IsCircular(const Impl* impl) const {
	auto positions = impl->GetReferencedCells();
	Cells outgoing_references(positions.begin(), positions.end());
	Cells visit;

	return IsCircularFormula(outgoing_references, visit);
}

bool Cell::IsCircularFormula(const Cells& outgoing_references, Cells& visit) const {
	if (outgoing_references.count(pos_) != 0) {
		return true;
	}

	for (Position pos : outgoing_references) {
		if (!pos.IsValid() || visit.count(pos) != 0) {
			continue;
		}

		visit.insert(pos);
		Cell* cell = MakeCell(pos);

		if (IsCircularFormula(cell->outgoing_references_, visit)) {
			return true;
		}
	}

	return false;
}

void Cell::UpdateReferences() {
	for (Position pos : outgoing_references_) {
		if (!pos.IsValid()) {
			continue;
		}

		Cell* cell = MakeCell(pos);
		cell->incoming_references_.erase(pos);
	}
	outgoing_references_.clear();

	for (Position pos : GetReferencedCells()) {
		if (!pos.IsValid()) {
			continue;
		}

		outgoing_references_.insert(pos);
		Cell* cell = MakeCell(pos);
		cell->incoming_references_.insert(pos);
	}
}

void Cell::InvalidCache() {
	cache_ = nullopt;
}

void Cell::InvalidAllDependentCaches(const Cells& incoming_references) {
	 for (Position pos : incoming_references) {
		if (!pos.IsValid()) {
			continue;
		}

		Cell* cell = MakeCell(pos);
		cell->InvalidCache();
		cell->InvalidAllDependentCaches(cell->incoming_references_);
	}
}

// --------------------EmptyImpl---------------------
Cell::EmptyImpl::EmptyImpl()
	: Impl() {
}

Cell::Value Cell::EmptyImpl::GetValue() const {
	return string();
}

string Cell::EmptyImpl::GetText() const {
	return string();
}

vector<Position> Cell::EmptyImpl::GetReferencedCells() const {
	return {};
}

// --------------------TextImpl---------------------
Cell::TextImpl::TextImpl(string content)
	: Impl()
	, content_(move(content)) {
}

Cell::Value Cell::TextImpl::GetValue() const {
	return (content_.front() != ESCAPE_SIGN) ? content_ : content_.substr(1);
}

string Cell::TextImpl::GetText() const {
	return content_;
}

vector<Position> Cell::TextImpl::GetReferencedCells() const {
	return {};
}

// --------------------FormulaImpl---------------------
Cell::FormulaImpl::FormulaImpl(string formula, const SheetInterface &sheet)
	: Impl()
	, formula_(move(ParseFormula(formula)))
	, sheet_(sheet) {
}

Cell::Value Cell::FormulaImpl::GetValue() const {
	auto result = formula_->Evaluate(sheet_);
	if(holds_alternative<double>(result)) {
		return get<double>(result);
	} else {
		return get<FormulaError>(result);
	}
}

string Cell::FormulaImpl::GetText() const {
	return FORMULA_SIGN + formula_->GetExpression();
}

vector<Position> Cell::FormulaImpl::GetReferencedCells() const {
	return formula_->GetReferencedCells();
}