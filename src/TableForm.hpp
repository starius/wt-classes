/*
 * wt-classes, utility classes used by Wt applications
 * Copyright (C) 2011 Boris Nagaev
 *
 * See the LICENSE file for terms of use.
 */

#ifndef WT_TABLE_FORM_HPP_
#define WT_TABLE_FORM_HPP_

#include <boost/function.hpp>

#include <Wt/WGlobal>
#include <Wt/WTable>

namespace Wt {

namespace Wc {

/** Set of user input widgets, with names and descriptions.

Example:
\code
    table_form = new TableForm();
    table_form->item(tr("age"), tr("age_description"), new WLineEdit());
\endcode

\section CSS
 - TableForm: wt_tableform
 - Header of new section: wt_tableform_header
 - Cell with name of field: wt_tableform_name
 - Cell with description of field: wt_tableform_description
 - Cell with input widget: wt_tableform_input

\ingroup util
*/
class TableForm : public WTable {
public:
    /** Constructor */
    TableForm(WContainerWidget* parent = 0);

    /** Start new section of form */
    void section(const WString& header);

    /** Add new item to field */
    WContainerWidget* item(const WString& name,
                           const WString& description = "",
                           WFormWidget* fw = 0,
                           WWidget* input = 0,
                           bool row = true);

    /** Show the row containing the input widget */
    void show(WWidget* input);

    /** Hide the row containing the input widget */
    void hide(WWidget* input);

    /** Apply a function to all items in order of addition.
    \attention Do not add widgets manually (e.g. by WTable->elementAt())
        if you are using this method.
    */
    void foreach(const boost::function<void(WWidget*)>& f);

private:
    WTableRow* parent_row_(WWidget* input);
    WWidget* input_at(int row);
};

}

}

#endif

