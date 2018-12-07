#include <iostream>
#include <string>
#include <cstddef>
#include "../inc/utils.hpp"

using namespace std;

/** -----------------------------------------------------
 * split a string based on the delimeter
 *
 * @params line, string to split
 * @params delim, character used as delimeter
 */
vector<string> split(const string line, const char delim){
  vector<string> sections;
  string delims = string() + delim + "\n\0";
  size_t pos_start = 0, pos_end = -1;
  while((pos_end = line.find_first_of(delims, pos_end + 1)) != string::npos){
    sections.emplace_back(line.substr(pos_start, pos_end - pos_start));
    pos_start = pos_end + 1;
  }
  sections.emplace_back(line.substr(pos_start));
  return sections;
}

/** -----------------------------------------------------
 * split a string based on the delimeter
 *
 * @params line, string to split
 * @params delim, character used as delimeter
 */
bool isFilter(string pred){
  MUST(pred.find('.') == pred.rfind('.')) // only one dot
  return true;
}
/** -----------------------------------------------------
 * validates that sections are as expected
 *
 * @params sections, Query's sections to validate
 * @returns true if valid, else false
 */
bool validateSections(const vector<string>& sections){
  MUST(sections.size() == 3)
  MUST(!sections[0].empty())
  MUST(!sections[1].empty())
  MUST(!sections[2].empty())
  return true;
}

/** -----------------------------------------------------
* validates that relation is as expected
*
* @params relation, Query's relation to validate
* @returns true if valid, else false
*/
bool validateRelation(const string& relation){
  MUST(!relation.empty()) // not empty
  MUST(relation.find_first_not_of("0123456789") == string::npos) // only digits
  // must relation.toInteger < relations read in phase 1
  return true;
}

/** -----------------------------------------------------
* validates that predicate is as expected
*
* @params predicate, Query's predicate to validate
* @returns true if valid, else false
*/
bool validatePredicate(const string& predicate){
  MUST(!predicate.empty()) // not empty
  size_t pos_operator, pos_dot;
  MUST((pos_operator=predicate.find_first_of("<>=")) == predicate.find_last_of("<>=")) // only one <,>,=
  string part1 = predicate.substr(0, pos_operator);
  string part2 = predicate.substr(pos_operator+1);
  MUST(part1.find_first_not_of(".0123456789") == string::npos) // only digits and dot
  MUST(part2.find_first_not_of(".0123456789") == string::npos) // only digits and dot
  bool at_least_one_column = false;
  if((pos_dot=part1.find('.')) != string::npos){
    MUST(pos_dot == part1.rfind('.')) // only one dot
    MUST(pos_dot != 0 && pos_dot != part1.size()-1) // dot not first or last
    at_least_one_column = true;
  }
  if((pos_dot=part2.find('.')) != string::npos){
    MUST(pos_dot == part2.rfind('.')) // only one dot
    MUST(pos_dot != 0 && pos_dot != part2.size()-1) // dot not first or last
    at_least_one_column = true;
  }
  MUST(at_least_one_column)

  return true;
}

/** -----------------------------------------------------
* validates that selector is as expected
*
* @params predicate, Query's predicate to validate
* @returns true if valid, else false
*/
bool validateSelector(const string& selector){
  size_t pos_dot;
  MUST(!selector.empty()) // not empty
  //LOG("Selector '%s' not empty OK!\n", selector.c_str())
  MUST(selector.find_first_not_of(".0123456789") == string::npos) // only digits and dot
  // LOG("Selector '%s' only digits and dot OK!\n", selector.c_str())
  MUST((pos_dot=selector.find('.')) != string::npos) // got a dot
  // LOG("Selector '%s' got a dot OK!\n", selector.c_str())
  MUST(pos_dot == selector.rfind('.')) // only one dot
  // LOG("Selector '%s' only one dot OK!\n", selector.c_str())
  MUST(pos_dot != 0 && pos_dot != selector.size()-1) // dot not first or last
  //LOG("Selector '%s' dot not first or last (%ld/%d) OK!\n", selector.c_str(), pos_dot,selector.size())
  return true;
}
