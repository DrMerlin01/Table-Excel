#pragma once

#include "common.h"
#include "formula.h"
#include "sheet.h"

#include <functional>
#include <unordered_set>

class Sheet;

class HashPosition {
public:
	size_t operator() (const Position& pos) const;

private:
	std::hash<int> i_hasher;
};

using Cells = std::unordered_set<Position, HashPosition>;

class Cell : public CellInterface {
public:
	Cell(Sheet& sheet, Position pos);

	~Cell();

	void Set(std::string text);

	void Clear();

	Value GetValue() const override;

	std::string GetText() const override;

	std::vector<Position> GetReferencedCells() const override;

	bool IsReferenced() const;

private:
	class Impl {
	public:
		virtual ~Impl() = default;

		virtual Value GetValue() const = 0;

		virtual std::string GetText() const = 0;

		virtual std::vector<Position> GetReferencedCells() const = 0;
	};

	class EmptyImpl : public Impl {
	public:
		EmptyImpl();

		Value GetValue() const override;

		std::string GetText() const override;

		std::vector<Position> GetReferencedCells() const override;
	};

	class TextImpl : public Impl {
	public:
		TextImpl(std::string content);

		Value GetValue() const override;

		std::string GetText() const override; 

		std::vector<Position> GetReferencedCells() const override;

	private:
		std::string content_;
	};

	class FormulaImpl : public Impl {
	public:
		FormulaImpl(std::string formula, const SheetInterface &sheet);

		Value GetValue() const override;

		std::string GetText() const override;

		std::vector<Position> GetReferencedCells() const override;

	private:
		std::unique_ptr<FormulaInterface> formula_;
		const SheetInterface &sheet_;
	};

	Sheet& sheet_;
	Position pos_;
	std::unique_ptr<Impl> impl_;
	Cells incoming_references_; //связи, от которых зависит эта ячейка
	Cells outgoing_references_; //обратные связи на другие ячейки из этой
	mutable std::optional<Value> cache_;

	Cell* MakeCell(Position pos) const;

	bool IsCircular(const Impl* impl) const;

	bool IsCircularFormula(const Cells& outgoing_references, Cells& visit) const;

	void UpdateReferences();

	void InvalidCache();

	void InvalidAllDependentCaches(const Cells& incoming_references);
};