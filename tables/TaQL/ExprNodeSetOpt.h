//# ExprNodeSetOpt.h: Classes representing an optimized set in table select expression
//# Copyright (C) 2022
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA

#ifndef TABLES_EXPRNODESETOPT_H
#define TABLES_EXPRNODESETOPT_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/tables/TaQL/ExprNodeRep.h>
#include <unordered_map>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

  // Forward Declarations
  class TableExprNodeSet;

  
  // <summary>
  // Abstract base class for optimized set representations
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSet
  // </prerequisite>

  // <synopsis>
  // This class is an optimized representation of an constant integer array set
  // with a large range of values used by the IN operator.
  // If applicable, TableExprLogicNode instantiates an object of this class
  // for sets with a value range of more than 16384.
  // <br>The representation is a std::unordered_map containing the array values.
  // <br>Note that a std::unordered_map is used instead of std::set because its
  // hashing mechanism makes it faster.
  // </synopsis>

  class TableExprNodeSetOptBase : public TableExprNodeRep
  {
  public:
    explicit TableExprNodeSetOptBase (const TableExprNodeRep& orig);
    virtual Bool contains (const TableExprId& id, Int64 value) override;
    virtual Bool contains (const TableExprId& id, Double value) override;
    virtual Bool contains (const TableExprId& id, String value) override;
    virtual MArray<Bool> contains (const TableExprId& id,
                                   const MArray<Int64>& value) override;
    virtual MArray<Bool> contains (const TableExprId& id,
                                   const MArray<Double>& value) override;
    virtual MArray<Bool> contains (const TableExprId& id,
                                   const MArray<String>& value) override;
    // Tell which key matches a value. -1 = no match.
    // The default implementations throw an exception.
    virtual Int64 find (Int64 value) const;
    virtual Int64 find (Double value) const;
    virtual Int64 find (String value) const;
  };


  // <summary>
  // An optimized representation of a discrete selection set.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSet
  // </prerequisite>

  // <synopsis>
  // This templated class is an optimized representation of an constant
  // integer or string array set used by the IN operator.
  // If applicable, TableExprLogicNode instantiates an object of this class.
  // <br>The representation is a std::unordered_map containing the array values
  // and the index in the array.
  // <br>Note that a std::unordered_map is used instead of std::map because its
  // hashing mechanism makes it faster.
  // </synopsis>

  template <typename T>
  class TableExprNodeSetOptUSet: public TableExprNodeSetOptBase
  {
  public:
    // Construct an empty set.
    TableExprNodeSetOptUSet (const TableExprNodeRep& orig, const Array<T>&);

    // Show the node.
    virtual void show (ostream& os, uInt indent) const override;

    // Where does a value occur in the set? -1 is no match.
    virtual Int64 find (T value) const override;

  private:
    std::unordered_map<T,Int64> itsMap;
  };


  // <summary>
  // An optimized representation of a selection set with continuous intervals.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSet
  // </prerequisite>

  // <synopsis>
  // This class is the base class for the optimized representation of a
  // constant selection set with continuous intervals.
  // </synopsis>

  class TableExprNodeSetOptContSetBase: public TableExprNodeSetOptBase
  {
  public:
    explicit TableExprNodeSetOptContSetBase (const TableExprNodeSet& orig);
  };


  // <summary>
  // An optimized representation of a selection set with continuous intervals.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSet
  // </prerequisite>

  // <synopsis>
  // This class is an optimized representation of a constant selection set
  // with continuous intervals using a mix of open and closed start and end.
  // If applicable, TableExprLogicNode instantiates an object of this class.
  // <br>The representation has std::vector objects containing the start
  // and end values. A lookup using std::upper_bound on the end values is done
  // to determine if a value is contained in one of the intervals.
  // <br>This templated class (as well as its derived classes) are instatiated
  // for Double and String.
  // </synopsis>

  template <typename T>
  class TableExprNodeSetOptContSet: public TableExprNodeSetOptContSetBase
  {
  public:
    TableExprNodeSetOptContSet (const TableExprNodeSet& orig,
                                const std::vector<T>& starts,
                                const std::vector<T>& ends,
                                const std::vector<Bool>& leftC,
                                const std::vector<Bool>& rightC);
    TableExprNodeSetOptContSet (const TableExprNodeSet& orig,
                                const std::vector<T>& starts,
                                const std::vector<T>& ends);
    // Show the node.
    virtual void show (ostream& os, uInt indent) const override;
    // Tell which interval contains a value. -1 = no match.
    virtual Int64 find (T value) const override;
    // Transform a set into an optimized one by ordering the intervals
    // and optionally combining adjacent intervals.
    // If not possible, an empty TENShPtr is returned.
    // It fill <src>rowNrs</src> with the row numbers of the intervals created.
    static TENShPtr transform (const TableExprNodeSet& set,
                               Bool combine=True);
    // Create the appropriate optimized OptContSet object.
    // Note that leftC and rightC do not need to have the same length as start/end.
    // If it is known that all intervals have the same leftC/rightC,
    // a single value suffices.
    static TENShPtr createOptSet (const TableExprNodeSet& set,
                                  const std::vector<T>& start,
                                  const std::vector<T>& end, 
                                  const std::vector<Bool>& leftC,
                                  const std::vector<Bool>& rightC);
    // Get the size (nr of intervals),
    size_t size() const
      { return itsStarts.size(); }
  protected:
    std::vector<T> itsStarts;
    std::vector<T> itsEnds;
    std::vector<Bool> itsLeftC;
    std::vector<Bool> itsRightC;
  };


  // <summary>
  // An optimized representation of a selection set with closed-closed intervals.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSetOptContSet
  // </prerequisite>

  // <synopsis>
  // This class is a further optimized version of TableExprNodeSetOptContSet
  // for continuous intervals all using a closed start and closed end.
  // It reduces the number of comparisons required.
  // </synopsis>

  template <typename T>
  class TableExprNodeSetOptContSetCC: public TableExprNodeSetOptContSet<T>
  {
  public:
    TableExprNodeSetOptContSetCC (const TableExprNodeSet& orig,
                                  const std::vector<T>& starts,
                                  const std::vector<T>& ends);
    // Show the node.
    virtual void show (ostream& os, uInt indent) const override;
    // Tell which interval contains a value. -1 = no match.
    virtual Int64 find (T value) const override;
  };

  
  // <summary>
  // An optimized representation of a selection set with open-closed intervals.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSetOptContSet
  // </prerequisite>

  // <synopsis>
  // This class is a further optimized version of TableExprNodeSetOptContSet
  // for continuous intervals all using an open start and closed end.
  // It reduces the number of comparisons required.
  // </synopsis>

  template <typename T>
  class TableExprNodeSetOptContSetOC: public TableExprNodeSetOptContSet<T>
  {
  public:
    TableExprNodeSetOptContSetOC (const TableExprNodeSet& orig,
                                  const std::vector<T>& starts,
                                  const std::vector<T>& ends);
    // Show the node.
    virtual void show (ostream& os, uInt indent) const override;
    // Tell which interval contains a value. -1 = no match.
    virtual Int64 find (T value) const override;
  };


  // <summary>
  // An optimized representation of a selection set with closed-open intervals.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSetOptContSet
  // </prerequisite>

  // <synopsis>
  // This class is a further optimized version of TableExprNodeSetOptContSet
  // for continuous intervals all using a closed start and open end.
  // It reduces the number of comparisons required.
  // </synopsis>

  template <typename T>
  class TableExprNodeSetOptContSetCO: public TableExprNodeSetOptContSet<T>
  {
  public:
    TableExprNodeSetOptContSetCO (const TableExprNodeSet& orig,
                                  const std::vector<T>& starts,
                                  const std::vector<T>& ends);
    // Show the node.
    virtual void show (ostream& os, uInt indent) const override;
    // Tell which interval contains a value. -1 = no match.
    virtual Int64 find (T value) const override;
  };


  // <summary>
  // An optimized representation of a selection set with open-open intervals.
  // </summary>

  // <use visibility=local>

  // <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
  // </reviewed>

  // <prerequisite>
  //# Classes you should understand before using this one.
  //   <li> TableExprNodeSetOptContSet
  // </prerequisite>

  // <synopsis>
  // This class is a further optimized version of TableExprNodeSetOptContSet
  // for continuous intervals all using an open  start and open end.
  // It reduces the number of comparisons required.
  // </synopsis>
  
  template <typename T>
  class TableExprNodeSetOptContSetOO: public TableExprNodeSetOptContSet<T>
  {
  public:
    TableExprNodeSetOptContSetOO (const TableExprNodeSet& orig,
                                  const std::vector<T>& starts,
                                  const std::vector<T>& ends);
    // Show the node.
    virtual void show (ostream& os, uInt indent) const override;
    // Tell which interval contains a value. -1 = no match.
    virtual Int64 find (T value) const override;
  };


} //# NAMESPACE CASACORE - END

#endif
