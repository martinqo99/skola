

#include "interpret.h"
#include "instructionList.h"

/**
 * Zjisti datovy typ adresy (pocita, adresa ukazuje na symbol).
 *
 * @param address void ukazatel na symbol.
 * @return dataovy typ symbolu.
 */
static DataType getAdressDataType(void * address) {
    return ((TSymbol *)address)->dataType;
}

/**
 * Interpretuje triadresny kod.
 *
 * @param instructionList ukazatel na seznam instrukci
 * @param returnValue vracena hodnota (cislo za return)
 * @return ErrorType
 */
ErrorType interpretCode(TInstructionList * instructionList, int * returnValue) {
    int c; // pro nacitani znaku z stdin
    TInstruction * currentInstruction;

    int * integer1, * integer2, * integer3;
    double * double1, * double2;
    TString * string1, * string2;

    activateFirstInstruction(instructionList);
    while (isActiveInstructionList(instructionList)) {
        currentInstruction = getActiveInstruction(instructionList);
        switch (currentInstruction->instructionType) {
            case IAdd:
                switch (getAdressDataType(currentInstruction->address3)) {
                    case DTInt:
                        ((TSymbol *)currentInstruction->address3)->value.intVal =
                            ((TSymbol *)currentInstruction->address1)->value.intVal +
                            ((TSymbol *)currentInstruction->address2)->value.intVal;
                        break;
                    case DTDouble:
                        ((TSymbol *)currentInstruction->address3)->value.doubleVal =
                            ((TSymbol *)currentInstruction->address1)->value.doubleVal +
                            ((TSymbol *)currentInstruction->address2)->value.doubleVal;
                        break;
                    default:
                        return ETCore;
                }
                break;
            case ISubtract:
                switch (getAdressDataType(currentInstruction->address3)) {
                    case DTInt:
                        ((TSymbol *)currentInstruction->address3)->value.intVal =
                            ((TSymbol *)currentInstruction->address1)->value.intVal -
                            ((TSymbol *)currentInstruction->address2)->value.intVal;
                        break;
                    case DTDouble:
                        ((TSymbol *)currentInstruction->address3)->value.doubleVal =
                            ((TSymbol *)currentInstruction->address1)->value.doubleVal -
                            ((TSymbol *)currentInstruction->address2)->value.doubleVal;
                        break;
                    default:
                        return ETCore;
                }
                break;
                break;
            case IMultiple:
                switch (getAdressDataType(currentInstruction->address3)) {
                    case DTInt:
                        ((TSymbol *)currentInstruction->address3)->value.intVal =
                            ((TSymbol *)currentInstruction->address1)->value.intVal *
                            ((TSymbol *)currentInstruction->address2)->value.intVal;
                        break;
                    case DTDouble:
                        ((TSymbol *)currentInstruction->address3)->value.doubleVal =
                            ((TSymbol *)currentInstruction->address1)->value.doubleVal *
                            ((TSymbol *)currentInstruction->address2)->value.doubleVal;
                        break;
                    default:
                        return ETCore;
                }
                break;
            case IDivide:
                switch (getAdressDataType(currentInstruction->address3)) {
                    case DTInt:
                        if (((TSymbol *)currentInstruction->address2)->value.intVal == 0) {
                            addError(&errors, ECDivideByZero, ETInterpreter, currentInstruction->lineNumber, NULL);
                            return ETInterpreter;
                        }
                        ((TSymbol *)currentInstruction->address3)->value.intVal =
                            ((TSymbol *)currentInstruction->address1)->value.intVal /
                            ((TSymbol *)currentInstruction->address2)->value.intVal;
                        break;
                    case DTDouble:
                        if (((TSymbol *)currentInstruction->address2)->value.doubleVal == 0) {
                            addError(&errors, ECDivideByZero, ETInterpreter, currentInstruction->lineNumber, NULL);
                            return ETInterpreter;
                        }
                        ((TSymbol *)currentInstruction->address3)->value.doubleVal =
                            ((TSymbol *)currentInstruction->address1)->value.doubleVal /
                            ((TSymbol *)currentInstruction->address2)->value.doubleVal;
                        break;
                    default:
                        return ETCore;
                }
                break;
            case IModulo:
                switch (getAdressDataType(currentInstruction->address3)) {
                    case DTInt:
                        ((TSymbol *)currentInstruction->address3)->value.intVal =
                            ((TSymbol *)currentInstruction->address1)->value.intVal %
                            ((TSymbol *)currentInstruction->address2)->value.intVal;
                        break;
                    default:
                        return ETCore;
                }
                break;
            case IAssign:
                switch (getAdressDataType(currentInstruction->address3)) {
                    case DTInt:
                        ((TSymbol *)currentInstruction->address3)->value.intVal =
                            ((TSymbol *)currentInstruction->address1)->value.intVal;
                        break;
                    case DTDouble:
                        ((TSymbol *)currentInstruction->address3)->value.doubleVal =
                            ((TSymbol *)currentInstruction->address1)->value.doubleVal;
                        break;
                    case DTString:
                        emptyString(&((TSymbol *)currentInstruction->address3)->value.stringVal);
                        copyString(&((TSymbol *)currentInstruction->address3)->value.stringVal,
                                &((TSymbol *)currentInstruction->address1)->value.stringVal);
                        break;
                    default:
                        return ETCore;
                }
                break;
            case IGoto:
                moveToInstruction(instructionList, (TListItem*)currentInstruction->address3);
                break;
            case IGotoIf:
                if (getAdressDataType(currentInstruction->address1) != DTInt) {
                    return ETCore;
                }
                if (((TSymbol *)currentInstruction->address1)->value.intVal != 0) {
                    moveToInstruction(instructionList, (TListItem*)currentInstruction->address3);
                }
                break;
            case IGotoIfNot:
                if (getAdressDataType(currentInstruction->address1) != DTInt) {
                    return ETCore;
                }
                if (((TSymbol *)currentInstruction->address1)->value.intVal == 0) {
                    moveToInstruction(instructionList, (TListItem*)currentInstruction->address3);
                }
                break;
            case IIntToDouble:
                if (getAdressDataType(currentInstruction->address1) != DTInt ||
                        getAdressDataType(currentInstruction->address3) != DTDouble) {
                    return ETCore;
                }
                ((TSymbol *)currentInstruction->address3)->value.doubleVal =
                    (double)((TSymbol *)currentInstruction->address1)->value.intVal;
                break;
            case ILabel:
                break;
            case ICin:
                switch (getAdressDataType(currentInstruction->address1)) {
                    case DTInt:
                        if (scanf("%d", &((TSymbol*)currentInstruction->address1)->value.intVal) == 0 ||
                                getchar() != '\n') {
                            addError(&errors, ECWrongInput, ETInterpreter, currentInstruction->lineNumber, NULL);
                            return ETInterpreter;
                        }
                        break;
                    case DTDouble:
                        if (scanf("%lf", &((TSymbol*)currentInstruction->address1)->value.doubleVal) == 0 ||
                                getchar() != '\n') {
                            addError(&errors, ECWrongInput, ETInterpreter, currentInstruction->lineNumber, NULL);
                            return ETInterpreter;
                        }
                        break;
                    case DTString:
                        emptyString(&((TSymbol*)currentInstruction->address1)->value.stringVal);
                        while ((c = getchar()) != EOF && c != '\n') {
                            addCharString(&((TSymbol*)currentInstruction->address1)->value.stringVal, c);
                        }
                        break;
                    default:
                        return ETCore;
                        break;
                }
                break;
            case ICout:
                switch (getAdressDataType(currentInstruction->address1)) {
                    case DTInt:
                        printf("%d", ((TSymbol*)currentInstruction->address1)->value.intVal);
                        break;
                    case DTDouble:
                        printf("%f", ((TSymbol*)currentInstruction->address1)->value.doubleVal);
                        break;
                    case DTString:
                        printf("%s", valueString(&((TSymbol*)currentInstruction->address1)->value.stringVal));
                        break;
                    default:
                        return ETCore;
                        break;
                }
                break;
            case IFind:
                if (getAdressDataType(currentInstruction->address1) != DTString ||
                        getAdressDataType(currentInstruction->address2) != DTString ||
                        getAdressDataType(currentInstruction->address3) != DTInt) {
                    return ETCore;
                }
                ((TSymbol *)currentInstruction->address3)->value.intVal =
                    boyerMoore(&((TSymbol *)currentInstruction->address1)->value.stringVal,
                            &((TSymbol *)currentInstruction->address2)->value.stringVal);
                break;
            case ISort:
                if (getAdressDataType(currentInstruction->address1) != DTString ||
                        getAdressDataType(currentInstruction->address3) != DTString) {
                    return ETCore;
                }
                sort(&((TSymbol *) currentInstruction->address1)->value.stringVal,
                        &((TSymbol *) currentInstruction->address3)->value.stringVal);
                break;
            case IConcat:
                if (getAdressDataType(currentInstruction->address1) != DTString ||
                        getAdressDataType(currentInstruction->address2) != DTString ||
                        getAdressDataType(currentInstruction->address3) != DTString) {
                    return ETCore;
                }
                concateneteStrings(&((TSymbol *)currentInstruction->address1)->value.stringVal,
                        &((TSymbol *)currentInstruction->address2)->value.stringVal,
                        &((TSymbol *)currentInstruction->address3)->value.stringVal);
                break;
            case ILength:
                if (getAdressDataType(currentInstruction->address1) != DTString ||
                        getAdressDataType(currentInstruction->address3) != DTInt) {
                    return ETCore;
                }
                ((TSymbol *)currentInstruction->address3)->value.intVal =
                    lenghtString(&((TSymbol *)currentInstruction->address1)->value.stringVal);
                break;
            case IEquals:
            case INotEquals:
            case ILessThen:
            case ILessEqualThen:
            case IGreaterThen:
            case IGreaterEqualsThen:
                if (getAdressDataType(currentInstruction->address1) !=
                    getAdressDataType(currentInstruction->address2) ||
                        getAdressDataType(currentInstruction->address3) != DTInt) {
                    return ETCore;
                }
                integer3 = &((TSymbol *)currentInstruction->address3)->value.intVal;
                switch (getAdressDataType(currentInstruction->address1)) {
                    case DTInt:
                        integer1 = &((TSymbol *)currentInstruction->address1)->value.intVal;
                        integer2 = &((TSymbol *)currentInstruction->address2)->value.intVal;
                        switch (currentInstruction->instructionType) {
                            case IEquals:
                                *integer3 = *integer1 == *integer2;
                                break;
                            case INotEquals:
                                *integer3 = *integer1 != *integer2;
                                break;
                            case ILessThen:
                                *integer3 = *integer1 < *integer2;
                                break;
                            case ILessEqualThen:
                                *integer3 = *integer1 <= *integer2;
                                break;
                            case IGreaterThen:
                                *integer3 = *integer1 > *integer2;
                                break;
                            case IGreaterEqualsThen:
                                *integer3 = *integer1 >= *integer2;
                                break;
                            default:
                                break;
                        }
                        break;
                    case DTDouble:
                        double1 = &((TSymbol *)currentInstruction->address1)->value.doubleVal;
                        double2 = &((TSymbol *)currentInstruction->address2)->value.doubleVal;
                        switch (currentInstruction->instructionType) {
                            case IEquals:
                                *integer3 = *double1 == *double2;
                                break;
                            case INotEquals:
                                *integer3 = *double1 != *double2;
                                break;
                            case ILessThen:
                                *integer3 = *double1 < *double2;
                                break;
                            case ILessEqualThen:
                                *integer3 = *double1 <= *double2;
                                break;
                            case IGreaterThen:
                                *integer3 = *double1 > *double2;
                                break;
                            case IGreaterEqualsThen:
                                *integer3 = *double1 <= *double2;
                                break;
                            default:
                                break;
                        }
                        break;
                    case DTString:
                        string1 = &((TSymbol *)currentInstruction->address1)->value.stringVal;
                        string2 = &((TSymbol *)currentInstruction->address2)->value.stringVal;
                        switch (currentInstruction->instructionType) {
                            case IEquals:
                                *integer3 = compareString(string1, string2) == 0;
                                break;
                            case INotEquals:
                                *integer3 = compareString(string1, string2) != 0;
                                break;
                            case ILessThen:
                                *integer3 = compareString(string1, string2) < 0;
                                break;
                            case ILessEqualThen:
                                *integer3 = compareString(string1, string2) <= 0;
                                break;
                            case IGreaterThen:
                                *integer3 = compareString(string1, string2) > 0;
                                break;
                            case IGreaterEqualsThen:
                                *integer3 = compareString(string1, string2) >= 0;
                                break;
                            default:
                                break;
                        }
                        break;
                    default:
                        return ETCore;
                }
                break;
            case IReturn:
                if (getAdressDataType(currentInstruction->address1) != DTInt) {
                    return ETCore;
                }
                *returnValue = ((TSymbol*)currentInstruction->address1)->value.intVal;
                return ETNoError;
                break;
            case IUndefined:
                printf("ITUndefined - ERROR");
                return ETCore;
                break;
        }
        nextInstruction(instructionList);
    }
    *returnValue = 0;
    return ETNoError;
}
