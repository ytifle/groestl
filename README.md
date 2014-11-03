groestl
=======

Implementacion para el calculo del efecto avalancha en Groestl


File list:

*.en files      - EMT Files
*.html files    - Console output of generated tests
Groestl-intermediate.c / Groestl-intermediate.h     - Implementation
brg_types.h     - Dependencies




To compile, just invoke gcc in this fashion:
gcc -o Groestl Groestl-intermediate.c
No special dependencies are needed (Except for brg_types.h).


To generate the EMT file for the rijndael test, use the following definitions:
```
#define TURN_OFF_PRINTING //Desactivar la impresion de datos generales.
#define TURN_OFF_PRINTING_ROUNDS //Desactivar la impresion de datos por ronda P/Q

//#define NO_PRINT_DIFF_MATRIX_EMT //Desactivar la impresion de salida EMT
#define NO_PRINT_DIFF_BINARY_MATRIX //Desactivar la impresion de matriz binaria
#define NO_PRINT_DIFF_MATRIX //Desactivar la salida de la matriz de variacion

#define NO_COMPARE_BLOCK //Desactivar la impresion de comparado de bloque
#define NO_COMPARE_PQ_ROUNDS //Desactivar el comparado de rondas de P/Q
//#define COMPARE_PQ_SUBROUNDS //Desactivar el comparado de subrondas de P/Q
#define NO_COMPARE_PQ //Desactivar el comparado de inicio/fin de datos de P/Q

//#define STATE_HEX_VALUES //Imprimir el valor de HEX

#define TEST_RIJNDAEL_PQ_BOXES
//#define TEST_GROSTL_ENCODING
```

Then, save the output using i.e.: ./Groestl > groestl_rijndael_1024.en
Open the output file in EMT, and press Ctrl+R (Run the worksheet).
When it finishes, export it to LaTeX or HTML.



To generate the full rijndael data (excluding subrounds), use this definitions:

```
//#define TURN_OFF_PRINTING //Desactivar la impresion de datos generales.
#define TURN_OFF_PRINTING_ROUNDS //Desactivar la impresion de datos por ronda P/Q

#define NO_PRINT_DIFF_MATRIX_EMT //Desactivar la impresion de salida EMT
//#define NO_PRINT_DIFF_BINARY_MATRIX //Desactivar la impresion de matriz binaria
//#define NO_PRINT_DIFF_MATRIX //Desactivar la salida de la matriz de variacion

//#define NO_COMPARE_BLOCK //Desactivar la impresion de comparado de bloque
#define NO_COMPARE_PQ_ROUNDS //Desactivar el comparado de rondas de P/Q
//#define COMPARE_PQ_SUBROUNDS //Desactivar el comparado de subrondas de P/Q
//#define NO_COMPARE_PQ //Desactivar el comparado de inicio/fin de datos de P/Q

//#define STATE_HEX_VALUES //Imprimir el valor de HEX

#define TEST_RIJNDAEL_PQ_BOXES
//#define TEST_GROSTL_ENCODING
```

Subrounds are exluded, because the generated file is BIG (>50MB).
To save it as html, you will need aha (https://github.com/theZiz/aha)
./Groestl | aha --black > rijndael_test_1024.html



To generate all the rounds and subrounds used to hash data, use this ones:

```
//#define TURN_OFF_PRINTING //Desactivar la impresion de datos generales.
//#define TURN_OFF_PRINTING_ROUNDS //Desactivar la impresion de datos por ronda P/Q

#define NO_PRINT_DIFF_MATRIX_EMT //Desactivar la impresion de salida EMT
//#define NO_PRINT_DIFF_BINARY_MATRIX //Desactivar la impresion de matriz binaria
//#define NO_PRINT_DIFF_MATRIX //Desactivar la salida de la matriz de variacion

//#define NO_COMPARE_BLOCK //Desactivar la impresion de comparado de bloque
//#define NO_COMPARE_PQ_ROUNDS //Desactivar el comparado de rondas de P/Q
#define COMPARE_PQ_SUBROUNDS //Desactivar el comparado de subrondas de P/Q
//#define NO_COMPARE_PQ //Desactivar el comparado de inicio/fin de datos de P/Q

//#define STATE_HEX_VALUES //Imprimir el valor de HEX

//#define TEST_RIJNDAEL_PQ_BOXES
#define TEST_GROSTL_ENCODING
```

Save it to html with aha (https://github.com/theZiz/aha), or just run it.
./Groestl | aha --black > groestl_test_1024.html


In a similar fashion, to obtain the EMT file, use this ones:
```
#define TURN_OFF_PRINTING //Desactivar la impresion de datos generales.
#define TURN_OFF_PRINTING_ROUNDS //Desactivar la impresion de datos por ronda P/Q

//#define NO_PRINT_DIFF_MATRIX_EMT //Desactivar la impresion de salida EMT
#define NO_PRINT_DIFF_BINARY_MATRIX //Desactivar la impresion de matriz binaria
#define NO_PRINT_DIFF_MATRIX //Desactivar la salida de la matriz de variacion

//#define NO_COMPARE_BLOCK //Desactivar la impresion de comparado de bloque
//#define NO_COMPARE_PQ_ROUNDS //Desactivar el comparado de rondas de P/Q
#define COMPARE_PQ_SUBROUNDS //Desactivar el comparado de subrondas de P/Q
//#define NO_COMPARE_PQ //Desactivar el comparado de inicio/fin de datos de P/Q

//#define STATE_HEX_VALUES //Imprimir el valor de HEX

//#define TEST_RIJNDAEL_PQ_BOXES
#define TEST_GROSTL_ENCODING
```

Then, save the output using i.e.: ./Groestl > groestl_1024.en
Open the output file in EMT, and press Ctrl+R (Run the worksheet).
When it finishes, export it to LaTeX or HTML.



I am deeply sorry for the mess, this is the price for not keeping to schedule.
