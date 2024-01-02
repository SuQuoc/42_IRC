
# include <string>
# include <vector>

std::vector<std::string> splitString(const std::string& input, const std::string& delimiter) 
{
    std::vector<std::string> result;

    size_t start = 0;
    size_t pos = input.find(delimiter);

    while (pos != std::string::npos) 
	{
        result.push_back(input.substr(start, pos - start));
        start = pos + delimiter.length();
        pos = input.find(delimiter, start);
    }
    result.push_back(input.substr(start));
    return result;
}



std::vector<std::string> splitIrcCmd(const std::string& input, const std::string& delimiter) 
{
    std::vector<std::string> result;

    size_t start = 0;
    size_t pos = input.find(delimiter);

    size_t trailing = input.find_last_of(':');

    while (pos != std::string::npos) 
	{
        result.push_back(input.substr(start, pos - start));
        start = pos + delimiter.length();
        if (input.at(pos) == ':')
        {
            result.push_back(input.substr(pos));
            break;
        }
        pos = input.find(delimiter, start);
    }
    result.push_back(input.substr(start));
    return result;
}
