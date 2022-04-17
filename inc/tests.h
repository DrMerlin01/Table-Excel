#pragma once

#include "common.h"

#include <iostream>

inline std::ostream& operator<<(std::ostream& output, Position pos);

inline Position operator"" _pos(const char* str, size_t);

inline std::ostream& operator<<(std::ostream& output, Size size);

inline std::ostream& operator<<(std::ostream& output, const CellInterface::Value& value);

void TestPositionAndStringConversion();

void TestPositionToStringInvalid();

void TestStringToPositionInvalid();

void TestEmpty();

void TestInvalidPosition();

void TestSetCellPlainText();

void TestClearCell();

void TestFormulaArithmetic();

void TestFormulaReferences();

void TestFormulaExpressionFormatting();

void TestFormulaReferencedCells();

void TestErrorValue();

void TestErrorDiv0();

void TestEmptyCellTreatedAsZero();

void TestFormulaInvalidPosition();

void TestPrint();

void TestCellReferences();

void TestFormulaIncorrect();

void TestCellCircularReferences();