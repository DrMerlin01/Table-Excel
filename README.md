# Table-Excel
A spreadsheet is a simplified analogue of existing solutions: a Microsoft Excel spreadsheet or Google Sheets. The table cells can contain text or formulas. Formulas, as in existing solutions, can contain cell indexes.
# Building and Run
```
  1. mkdir TableExcelBuild && cd TableExcelBuild 
  2. cmake .. -DCMAKE_BUILD_TYPE=Release
  3. cmake --build . 
  4. Start ./spreadsheet or spreadsheet.exe
```
# System requirements and Stack
  1. C++17
  2. GCC version 8.1.0
  3. ANTLR4 4.9.3
  4. GRUN
  5. Java SE Runtime Environment 8 
  6. Java SE Development Kit 8
  7. Cmake 3.21.2 (minimal 3.8)
# Future plans
  1. Implement an interface for inserting and deleting rows and columns
  2. Implement an interface for working with the user
