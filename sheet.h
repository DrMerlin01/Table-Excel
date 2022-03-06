#pragma once

#include "cell.h"
#include "common.h"

#include <functional>

class Cell;

class Sheet : public SheetInterface {
public:
	~Sheet();

	void SetCell(Position pos, std::string text) override;

	const CellInterface* GetCell(Position pos) const override;

	CellInterface* GetCell(Position pos) override;

	void ClearCell(Position pos) override;

	Size GetPrintableSize() const override;

	void PrintValues(std::ostream& output) const override;

	void PrintTexts(std::ostream& output) const override;

	const Cell* GetCellByIndex(const Position pos) const;

	Cell* GetCellByIndex(const Position pos);

private:
	void PrintCells(std::ostream &output, const std::function<void(const CellInterface &)> &printCell) const;

	std::vector<std::vector<std::unique_ptr<Cell>>> cells_;
};