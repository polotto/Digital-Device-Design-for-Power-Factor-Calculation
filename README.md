# Digital-Device-Design-for-Power-Factor-Calculation

## Introduction / Introdução

This is my graduation thesis complete. My work was create disign a digial power factor meter using Tiva C microcontroller from Texas Instruments and ACS712 hall effect sensor. All the information about the measurement is displayed in a graphic serial monitor running on the computer.
All the Tiva C code was written in C language and the serial monitor was writter in Qt C++. The simulation code was written in Matlab and C ANSI.
To my thesis text, I used Latex code. All the thesis text was written in portuguse, if someone want to translate, please, fell free to do it.

## Content of This Repository

* Circuit Design
  * Eagle Board PDF Exported: Board design exported to PDF.
  * Eagle Board Project: Board design project made with Eagle software.
  * Multisim Simulation: Circuit simulation made with Multisim.
* Codes
  * CCS Texas Tiva C Code: Code in C written to Tiva C board, I used the CCS Texas Instruments Compiler.
  * Digital Fourier Transformer Tests: Somes tests that I made to measure the FFT performance before to implemente in the Tiva C board.
  * RS232 Terminal QT C++: Graph desktop program to see the voltage and current measured by the device. 
  * Simulations
    * Functions Test: Test of the algorithms written in C.
    * Matlab: Algorithm tests.
* Thesis Latex: All my thesis latex content.
* Thesis_Polotto_TD_COELE_2015_2_026_Portuguese: PDF of my thesis in portuguse.

## Thesis Summary in Inglish

### Digital Device Design for Power Factor Calculation

The power factor (PF) is one of the parameters related to power quality whose inadequate values cause: increasing internal power losses and voltage drops in the facility; reduction of usable capacity of transformers and engines; heated drivers; among others. Aiming for an application in the power quality area, this study proposes the development of a device capable of measuring $PF$ of linear and nonlinear circuits (particularly single-phase rectifiers with resistive loads). The prototype will consist of a microcontroller, a Hall effect sensor to measure current and a circuit for conditioning the voltage and current signals. The embedded software developed for the system is responsible for sampling the involved quantities and, through numerical methods, calculate PF and make it accessible to the user on an interface. The operation of meter is compared with simulation data.

## Thesis Summary in Portuguses

### Projeto de Dispositivo Digital para Cálculo do Fator de Potência

O fator de potência (FP) é um dos parâmetros relacionados à qualidade de energia, cujos valores inadequados ocasionam: aumento das perdas elétricas internas e quedas de tensão na instalação; redução do aproveitamento da capacidade dos transformadores e motores; condutores aquecidos; entre outras. Visando uma aplicação na área de qualidade de energia, esse trabalho propõe o desenvolvimento de um dispositivo capaz de mensurar o FP de circuitos lineares e não lineares (especificamente retificadores monofásicos com cargas resistivas). O projeto do protótipo é composto por um microcontrolador, sensor de efeito _Hall_ para medida de corrente e um circuito para condicionamento dos sinais de tensão e corrente. O _software_ desenvolvido e embarcado no sistema é responsável por amostrar as grandezas envolvidas e calcular o FP, através de métodos numéricos, e torná-lo acessível ao usuário em uma interface. O funcionamento do medidor é comparado com dados de simulações.