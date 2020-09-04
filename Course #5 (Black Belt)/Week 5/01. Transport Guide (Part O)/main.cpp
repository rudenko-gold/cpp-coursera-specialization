#include "descriptions.h"
#include "json.h"
#include "requests.h"
#include "sphere.h"
#include "transport_catalog.h"
#include "utils.h"
#include "transport_catalog.pb.h"

#include <iostream>
#include <fstream>
#include <string_view>

using namespace std;

string ReadFileData(const string& file_name) {
    ifstream file(file_name, ios::binary | ios::ate);
    const ifstream::pos_type end_pos = file.tellg();
    file.seekg(0, ios::beg);

    string data(end_pos, '\0');
    file.read(&data[0], end_pos);
    return data;
}

int main(int argc, const char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: transport_catalog_part_o [make_base|process_requests]\n";
        return 5;
    }

    const string_view mode(argv[1]);

    if (mode == "make_base") {
        const auto input_doc = Json::Load(cin);
        const auto& input_map = input_doc.GetRoot().AsMap();

        const TransportCatalog db(
                Descriptions::ReadDescriptions(input_map.at("base_requests").AsArray())
                //input_map.at("routing_settings").AsMap(),
                //input_map.at("render_settings").AsMap()
        );

        Serialized::TransportCatalog result = db.Serialize();
        std::string file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();

        fstream fout;
        fout.open(file_name, ios::out | ios::trunc | ios::binary);

        result.SerializeToOstream(&fout);

        fout.close();
    } else if (mode == "process_requests") {
        const auto input_doc = Json::Load(cin);
        const auto& input_map = input_doc.GetRoot().AsMap();
        std::string file_name = input_map.at("serialization_settings").AsMap().at("file").AsString();

        fstream fin;
        fin.open(file_name, ios::in | ios::binary);

        Serialized::TransportCatalog serialized_transport_catalog;
        serialized_transport_catalog.ParseFromIstream(&fin);

        fin.close();

        const TransportCatalog db(serialized_transport_catalog);

        Json::PrintValue(
                Requests::ProcessAll(db, input_map.at("stat_requests").AsArray()),
                cout
        );
        cout << endl;
    }

    return 0;
}
