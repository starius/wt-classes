/**
 * wt-classes is licensed under the GNU GPL Version 2.
 * Copyright (C) 2011 Boris Nagaev
 *
 * wt-classes is licensed under the GNU GPL Version 2.
 * Other versions of the GPL do not apply.
 * See the LICENSE file for terms of use.
 */

#ifndef WT_TABLE_FORM_HPP_
#define WT_TABLE_FORM_HPP_

#include <Wt/WGlobal>
#include <Wt/WTable>

namespace Wt {

/** Set of user input widgets, with names and descriptions.

Example:
\code
    table_form = new Wt::TableForm();
    table_form->item(tr("age"), tr("age_description"), new Wt::WLineEdit());
\endcode

\section CSS
 - TableForm: wt_tableform
 - Header of new section: wt_tableform_header
 - Cell with name of field: wt_tableform_name
 - Cell with description of field: wt_tableform_description
 - Cell with input widget: wt_tableform_input

*/
class TableForm : public Wt::WTable {
public:
    /** Constructor */
    TableForm(Wt::WContainerWidget* parent = 0);

    /** Start new section of form */
    void section(const Wt::WString& header);

    /** Add new item to field */
    Wt::WContainerWidget* item(const Wt::WString& name,
                               const Wt::WString& description = "",
                               Wt::WFormWidget* fw = 0, Wt::WWidget* input = 0, bool row = true);

    /** Show the row containing the input widget */
    void show(Wt::WWidget* input);

    /** Hide the row containing the input widget */
    void hide(Wt::WWidget* input);

private:
    Wt::WTableRow* parent_row_(Wt::WWidget* input);
};

}

#endif

