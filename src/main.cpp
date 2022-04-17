#include "../inc/test_runner_p.h"
#include "../inc/tests.h"

using namespace std;

int main() {
	TestRunner tr;

	RUN_TEST(tr, TestPositionAndStringConversion);
	RUN_TEST(tr, TestPositionToStringInvalid);
	RUN_TEST(tr, TestStringToPositionInvalid);
	RUN_TEST(tr, TestEmpty);
	RUN_TEST(tr, TestInvalidPosition);
	RUN_TEST(tr, TestSetCellPlainText);
	RUN_TEST(tr, TestClearCell);
	RUN_TEST(tr, TestFormulaArithmetic);
	RUN_TEST(tr, TestFormulaReferences);
	RUN_TEST(tr, TestFormulaExpressionFormatting);
	RUN_TEST(tr, TestFormulaReferencedCells);
	RUN_TEST(tr, TestErrorValue);
	RUN_TEST(tr, TestErrorDiv0);
	RUN_TEST(tr, TestEmptyCellTreatedAsZero);
	RUN_TEST(tr, TestFormulaInvalidPosition);
	RUN_TEST(tr, TestPrint);
	RUN_TEST(tr, TestCellReferences);
	RUN_TEST(tr, TestFormulaIncorrect);
	RUN_TEST(tr, TestCellCircularReferences);

	return 0;
}