#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <stdlib.h>
#include <argp.h>
#include <limits.h>
#include "defs.h"

#define WHITESPACE    " \t\n\r"

static const str_list_t categories{"alarm", "connection", "telemetry", "vlan", "weather"};
static const str_list_t type_choices{"redis", "zeromq", "nano"};

/**
 * @brief Checks that a string starts with a given prefix.
 *
 * @param s The string to check
 * @param prefix A string that s could be starting with
 *
 * @return If s starts with prefix then return a pointer inside s right
 *         after the end of prefix.
 *         NULL otherwise
 */
static inline char * startswith(const char *s, const char *prefix)
{
    size_t l = strlen(prefix);
    if (strncmp(s, prefix, l) == 0) return (char *)s + l;

    return NULL;
}

static str_list_t split(const std::string& s, char delimiter=',')
{
   str_list_t tokens;
   std::string token;
   std::istringstream token_stream(s);
   while (std::getline(token_stream, token, delimiter))
   {
      tokens.push_back(token);
   }
   return tokens;
}

static std::string join( const str_list_t& elements, const char* const separator = ", ")
{
    switch (elements.size())
    {
        case 0: return "";
        case 1: return elements[0];
        default: ;
    }

    std::ostringstream os;
    std::copy(elements.begin(), elements.end()-1, std::ostream_iterator<std::string>(os, separator));
    os << *elements.rbegin();
    return os.str();
}

struct arguments_c
{
    std::string         cnf_file; // Configuration file
    str_list_t          cat_lst;  // alarm, connection, telemetry, vlan, weather
    str_list_t          url_lst;  // Publisher's URL
    std::string         type;     // redis, zeromq, nano
    unsigned long int   num;

    arguments_c(int argc, char **argv, const char * doc, bool publisher) : num(1000), cnf_file("")
    {
        std::string cats  = join(categories, ", ");
        std::string types = join(type_choices, ", ");

        std::string cat_help  = "Category of data exchanged over the pub/sub channel. Choices: " + cats;
        std::string type_help = "Pub/Sub type. Choices: " + types;

        struct argp_option options[] =
        {
            { "cnf",  'c', "FILE",   0, "Configuration file" },
            { "type", 't', "STRING", 0, type_help.c_str() },
            { NULL }
        };
        struct argp argp = { options, parse_opt, "", doc };

        argp_parse(&argp, argc, argv, 0, 0, this);

        int rc = 0;

        if (std::find(std::begin(type_choices), std::end(type_choices), type) == std::end(type_choices))
        {
            std::cerr << "Invalid argument: type=\"" << type <<"\". Choices are: " << types << std::endl;
            rc = -1;
        }

        FILE * file;
        if ((cnf_file == "") || (NULL == (file = fopen(cnf_file.c_str(), "re"))))
        {
            std::cerr << "Invalid argument: cnf=\"" << cnf_file <<"\". Specify a valid configuration file." << std::endl;
            rc = -1;
        }
        else
        {
            if (file)
            {
                char    line[LINE_MAX];
                char  * p;
                char  * s;
                while (NULL != (p = fgets(line, sizeof line, file)))
                {
                    p += strspn(p, WHITESPACE);
                    p[strcspn(p, WHITESPACE)] = '\0';
                    if (NULL != (s = startswith(p, "category-list")))
                    {
                        s += strspn(s, " \t=");
                        cat_lst = split(s);
                    }
                    else if (NULL != (s = startswith(p, "category")))
                    {
                        s += strspn(s, " \t=");
                        cat_lst = split(s);
                    }
                    else if (NULL != (s = startswith(p, "num-events")))
                    {
                        s += strspn(s, " \t=");
                        num = strtoul(s, NULL, 10);
                    }
                    else if (NULL != (s = startswith(p, "url-list")))
                    {
                        s += strspn(s, " \t=");
                        url_lst = split(s);
                    }
                    else if (NULL != (s = startswith(p, "url")))
                    {
                        s += strspn(s, " \t=");
                        url_lst.clear();
                        url_lst.push_back(s);
                    }
                    else
                    {
                        std::cerr << "Unknown config parameter: \"" << s <<"\"." << std::endl;
                    }
                }

                fclose(file);
            }
        }

        if ((type == "zeromq") || (type == "nano"))
        {
            if (cat_lst.size() == 0)
            {
                std::cerr << "Invalid config parameter: url=\"\". Choices are: ipc://[path-name] or tcp://[addr:port]" << std::endl;
                rc = -1;
            }
            else
            {
                for (auto url : url_lst)
                {
                    if ((url.find("ipc://") != 0) && (url.find("tcp://") != 0))
                    {
                        std::cerr << "Invalid config parameter: url=\"" << url <<"\". Choices are: ipc://[path-name] or tcp://[addr:port]" << std::endl;
                        rc = -1;
                    }
                }
            }
        }

        if (cat_lst.size() == 0)
        {
            std::cerr << "Invalid config parameter: category=\"\". Choices are: " << cats << std::endl;
            rc = -1;
        }
        else
        {
            for (auto cat : cat_lst)
            {
                if (std::find(std::begin(categories), std::end(categories), cat) == std::end(categories))
                {
                    std::cerr << "Invalid config parameter: category=\"" << cat <<"\". Choices are: " << cats << std::endl;
                    rc = -1;
                }
            }
        }

        if (publisher && (num == 0))
        {
            std::cerr << "Invalid config parameter: num-events=" << num <<". Value must be greater than 0" << std::endl;
            rc = -1;
        }

        if (rc != 0)
        {
            exit(EXIT_FAILURE);
        }
    }

private:
    static error_t parse_opt(int key, char *arg, struct argp_state *state)
    {
        /* Get the input argument from argp_parse, which we
           know is a pointer to our arguments_c structure. */
        arguments_c * arguments_p = (arguments_c *)state->input;

        switch (key)
        {
        case 'c': arguments_p->cnf_file = arg; break;
        case 't': arguments_p->type     = arg; break;

        case ARGP_KEY_ARG: break;
        case ARGP_KEY_END: break;

        default:  return ARGP_ERR_UNKNOWN;
        }
        return 0;
    }
};

