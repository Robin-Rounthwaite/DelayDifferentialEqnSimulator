#include "csvw.hpp"
#include "util/color.hpp"

#include <cfloat> // For FLT_MAX as an internal error code
#include <iostream>
using namespace std;



csvw::csvw(const std::string& pcfFileName, const bool& pcfWriteDoc, const string& pcfChildDoc)
{
    // Close any previously open file
    if (iFile.is_open())
        iFile.close();
    
    // Open new file
    iFile.open(pcfFileName);
    
    // Check if open successful
    if (!iFile.is_open())
    {
        cout << color::set(color::RED) << "CSV file output failed. CSV file \'" <<
            pcfFileName << "\' unable to be written to." << color::clear() << endl;
    }

    if (pcfWriteDoc)
    {
        // Write parent documentation
        iFile << "# CSV Specification\n";
        iFile << "#   Ignored by the file readers are:\n";
        iFile << "#     (1) Empty cells / Blank rows / Whitespace\n";
        iFile << "#     (2) Comment whose rows always begin with a \'#\'\n";
        iFile << "#         For best results delete all comments before loading "
            "this file into any Excel-like program.\n";
        iFile << "#     (3) Any cell which does not conform to the scientific "
            "notation format 3.14e-41 or simple whole numbers and decimals\n";
        iFile << "#         Often times cells which do not contain numbers are "
            "intended to be column headers. These are not parsed by the simulation "
            "and can technically be modified by the users as they wish.\n";
        iFile << "#         It is futile to add/remove/modify the column headers "
            "with the expectation of changing the program's behavior. Data must "
            "be entered in the default order for it to be parsed correctly.\n";
        iFile << "# None of these comments include commas because it messes with "
            "the column widths when loaded into Excel-like programs.\n";
        iFile << "# For more information and examples see README.md section 2.2.0\n";

        // Write child documentation
        iFile << pcfChildDoc;
    }
}


csvw::~csvw()
{
    iFile.close();
}


void csvw::add_div(const string& pcfDiv)
{
    iFile << pcfDiv;
}


void csvw::add_data(const RATETYPE& pcfRate)
{
    iFile << pcfRate;
    add_div(",");
}
