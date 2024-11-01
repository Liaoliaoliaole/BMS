*** Settings ***
Library    SerialLibrary.py

*** Variables ***
${PORT}       COM8
${BAUDRATE}   9600
${TIMEOUT}    10

*** Test Cases ***
Test Device Status Command
    Open Serial Port    ${PORT}    ${BAUDRATE}
    Write To Serial     ~STATUS\n
    ${response}=        Read From Serial    ${TIMEOUT}
    Sleep    0.1s
    Should Be Equal As Strings    ${response}    Device is working
    Close Serial Port

Test Device Working Confirmation Command
    Open Serial Port    ${PORT}    ${BAUDRATE}
    Write To Serial     ~MAKESURE\n
    ${response}=        Read From Serial    ${TIMEOUT}
    Sleep    0.1s
    Should Be Equal As Strings    ${response}    Device is indeed working
    Close Serial Port

Test Device Status Command 2
    Open Serial Port    ${PORT}    ${BAUDRATE}
    Write To Serial     ~STATUS\n
    ${response}=        Read From Serial    ${TIMEOUT}
    Sleep    0.1s
    Should Be Equal As Strings    ${response}    Device is working

    Close Serial Port