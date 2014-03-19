#include "updatequery.h"


UpdateQuery::UpdateQuery(Table table, list<Condition> conditions, list<SetAction> setActions):table(table), conditions(conditions), setActions(setActions) {
}