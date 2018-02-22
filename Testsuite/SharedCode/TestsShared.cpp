//          Copyright Benjamin Southall 2018.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include "TestsShared.h"
#include <sstream>


Logger::Logger(ostream &where, unsigned verbosity, bool logPasses, bool logBorings)
    : fStream(where),
      fNullStream(&fNullBuffer),
      fVerbosity(verbosity),
      fTestNumber(0),
      fLogPasses(logPasses),
      fLogBorings(logBorings)
{}


std::ostream& Logger::pass(int number)
{
    fInfoStream.str(""); //Clear the logs as we succeeded

    if (number > 0){
        fCurrentPasses += number;
        if (fLogPasses)
            return fStream << green << "Pass - ";
        return fNullStream;
    }
    fCurrentPasses -= number;
    return fNullStream;
}

std::ostream& Logger::bore(int number)
{
    fInfoStream.str(""); //Clear the logs as we succeeded

    if (number > 0){
        fCurrentBorings += number;
        if (fLogBorings)
            return fStream << yellow << "Pass - ";
        return fNullStream;
    }
    fCurrentBorings -= number;
    return fNullStream;
}

std::ostream& Logger::fail(int number)
{
    if (number > 0){

        fCurrentFailures += number;
        auto logs = fInfoStream.str();
        fInfoStream.str("");
        return fStream << logs << red << "Fail - ";
    }
    fCurrentFailures -= number;
    return fNullStream;
}

std::ostream& Logger::info()
{
    return fInfoStream << normal;
}


void Logger::startTest(std::string function, std::string header)
{
    fStream << "\n\n" << blue << "====== Test No. " << fTestNumber << " " << function << endl << header << normal;
    fCurrentPasses = fCurrentBorings = fCurrentFailures = 0;
}


bool Logger::dumpTest(unsigned testNumber)
{
    auto results = fCountsList[testNumber];
    unsigned passes = std::get<0>(results),
        borings = std::get<1>(results),
        failures = std::get<2>(results);

    fStream << endl << endl << ( !failures ? green : red) << "Test " << testNumber
            << "\tTOTAL: " << passes + borings + failures
            << ":\tPass: " << passes << "\tBoring: " << borings << "\tFail: " << failures
            << fixed << setprecision(0)
            << "\tPass rate: " << 100 * (float)(borings + passes) / (failures + borings  + passes) << "%"
            << normal << defaultfloat << endl;

    return !failures;
}

void Logger::endTest()
{
    fCountsList.push_back(std::make_tuple(fCurrentPasses, fCurrentBorings, fCurrentFailures));
    dumpTest(fTestNumber++);

}

bool Logger::dumpTests()
{
    fStream <<
        " _____                                             \n"
        "/  ___|                                           \n"
        "\\ `--. _   _ _ __ ___  _ __ ___   __ _ _ __ _   _ \n"
        " `--. \\ | | | '_ ` _ \\| '_ ` _ \\ / _` | '__| | | |\n"
        "/\\__/ / |_| | | | | | | | | | | | (_| | |  | |_| |\n"
        "\\____/ \\__,_|_| |_| |_|_| |_| |_|\\__,_|_|   \\__, |\n"
        "                                             __/ |\n"
        "                                            |___/ \n"

    << endl << endl << endl << fixed << setprecision(0);

    unsigned passes = 0, borings = 0, failures = 0;

    for (auto i = 0; i != fCountsList.size(); i++){
        passes += std::get<0>(fCountsList[i]);
        borings += std::get<1>(fCountsList[i]);
        failures += std::get<2>(fCountsList[i]);

        dumpTest(i);
    }


    fStream << endl << endl <<( !failures ? green : red) << "TOTAL: " << passes + borings + failures
            << ":\tPass: " << passes << "\tBoring: " << borings << "\tFail: " << failures
            << fixed << setprecision(0)
            << "\tPass rate: " << 100 * (float)(borings + passes) / (failures + borings  + passes) << "%"
            << normal << defaultfloat << endl;

    return failures == 0;

}



void testObjectsAgainstTwoTypes(
        const std::list<ObjectIF*>& objects,
        const std::list<const std::type_info*>& types1,
        const std::list<const std::type_info*>& types2,
        std::function<void(const char *, const void*, const void*)> outputFunction,
        Logger &logger
)
{
    for (auto object : objects){
        for(
            auto type1 = types1.cbegin(), type2 = types2.cbegin();
            (type1 != types1.cend()) && (type2 != types2.cend());
            type1++, type2++
        ){
            //Fixme! use logger
            logger.info()
                << typeName(object->staticType()) << "[" << typeName(object->dynamicType()) << "] -> ("
                << typeName(*type1) << ") - (" << typeName(*type2) << ")"
                << "\n\t";

            outputFunction("nc2", object->newCast(**type1), object->newCast(**type2));

            logger.info() << "\n\t";

            outputFunction("nc3", object->newCast3Arg(**type1), object->newCast3Arg(**type2));

        }
    }
}


