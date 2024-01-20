// This may look like C code, but it's really -*- C++ -*-
/*
 * Copyright (C) 2008 Emweb bvba, Kessel-Lo, Belgium.
 *
 * See the LICENSE file for terms of use.
 */
#ifndef FILETREETABLE_H_
#define FILETREETABLE_H_

#include <Wt/WTreeTable>
#include <Wt/WTree>
#include <Wt/WTreeNode>
#include <Wt/WTreeTableNode>

#include <boost/filesystem/path.hpp>
namespace Wt {
namespace Wc {

/**
 * \defgroup FileTreeTable widget that displays a file system tree
 */
/*@{*/

/*! \brief A tree table that displays a file tree.
 *
 * The table allows one to browse a path, and all its subdirectories,
 * using a tree table. In addition to the file name, it shows file size
 * and modification date.
 *
 * The table use FileTreeTableNode objects to display the actual content
 * of the table. 
 *
 * The tree table uses the LazyLoading strategy of WTreeNode to dynamically
 * load contents for the tree.
 *
 * This widget was adapted from the %Wt File Explorer example.
 */
class FileTreeTable : public Wt::WTreeTable
{
public:
  /*! \brief Construct a new FileTreeTable.
   *
   * Create a new FileTreeTable to browse the given path.
   * \param path directory to display
   * \param suffix if non-empty, only display files with this suffix
   * \param parent optional Parent widget
   */
    FileTreeTable(const boost::filesystem::path& path, const std::string &suffix="",
		Wt::WContainerWidget *parent=0);
};
}
}

/*@}*/

#endif // FILETREETABLE_H_
