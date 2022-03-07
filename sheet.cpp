#include "sheet.h"
#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std;

Sheet::~Sheet() {
	for (auto &row : cells_) {
		for (auto &cell : row) {
			if (cell) {
				cell->Clear();
			}
		}
	}
}

void Sheet::SetCell(Position pos, string text) {
	IsValidPosition(pos, "SetCell");

	cells_.resize(max(pos.row + 1, static_cast<int>(cells_.size())));
	cells_[pos.row].resize(max(pos.col + 1, static_cast<int>(cells_[pos.row].size())));

	auto &cell = cells_[pos.row][pos.col];
	if (!cell) {
		cell = make_unique<Cell>(*this, pos);
	}
	cell->Set(move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
	return GetCellByIndex(pos);
}

CellInterface* Sheet::GetCell(Position pos) {
	return GetCellByIndex(pos);
}

void Sheet::ClearCell(Position pos) {
	IsValidPosition(pos, "ClearCell");

	if (pos.row < static_cast<int>(cells_.size()) && pos.col < static_cast<int>(cells_[pos.row].size())) {
		auto &cell = cells_[pos.row][pos.col];
		if (cell) {
			cell->Clear();
		}
	}
}

Size Sheet::GetPrintableSize() const {
	Size size;

	for (int row = 0; row < static_cast<int>(cells_.size()); ++row) {
		for (int col = static_cast<int>(cells_[row].size()) - 1; col >= 0; --col) {
			const auto &cell = cells_[row][col];
			if (cell) {
				if (!cell->GetText().empty()) {
					size.rows = max(size.rows, row + 1);
					size.cols = max(size.cols, col + 1);
					break;
				}
			}
		}
	}

	return size;
}

void Sheet::PrintValues(ostream& output) const {
	PrintCells(output, 
			   [&](const CellInterface &cell) { 
				   visit([&](const auto &value) { output << value; }, 
						 cell.GetValue()); 
			   });
}

void Sheet::PrintTexts(ostream& output) const {
	PrintCells(output, [&output](const CellInterface &cell) { output << cell.GetText(); });
}

const Cell* Sheet::GetCellByIndex(const Position pos) const {
	IsValidPosition(pos, "GetCellByIndex");

	if (pos.row >= static_cast<int>(cells_.size()) || 
		pos.col >= static_cast<int>(cells_[pos.row].size()) ||
		cells_[pos.row][pos.col]->GetText().empty()) {
		return nullptr;
	} else {
		return cells_[pos.row][pos.col].get();
	}
}

Cell* Sheet::GetCellByIndex(const Position pos) {
	return const_cast<Cell*>(static_cast<const Sheet &>(*this).GetCellByIndex(pos));
}

void Sheet::PrintCells(ostream &output, const function<void(const CellInterface &)> &printCell) const {
	auto size = GetPrintableSize();
	for (int row = 0; row < size.rows; ++row) {
		for (int col = 0; col < size.cols; ++col) {
			if (col > 0) {
				output << '\t';
			}
			if (col < static_cast<int>(cells_[row].size())) {
				if (const auto &cell = cells_[row][col]) {
					printCell(*cell);
				}
			}
		}
		output << '\n';
	}
}

void Sheet::IsValidPosition(const Position pos, const string& function_name) const {
	if (!pos.IsValid()) {
		throw InvalidPositionException("Invalid position passed to " + function_name + "!!!");
	}
}

unique_ptr<SheetInterface> CreateSheet() {
	return make_unique<Sheet>();
}