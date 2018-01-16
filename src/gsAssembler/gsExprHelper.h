/** @file gsExprHelper.h

    @brief Generic expressions helper

    This file is part of the G+Smo library.

    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

    Author(s): A. Mantzaflaris
*/

#pragma once

#include <gsAssembler/gsExpressions.h>

namespace gismo
{
/**
   Class holding an expression environment
 */
template<class T>
class gsExprHelper
{
private:
    gsExprHelper(const gsExprHelper &);

    gsExprHelper() : mesh_ptr(NULL)
    { mutVar.setData(mutData); }

private:
    typedef std::map<const gsFunctionSet<T>*,gsFuncData<T> > FunctionTable;
    typedef typename FunctionTable::iterator ftIterator;
    typedef typename FunctionTable::const_iterator const_ftIterator;

    // variable/space list
    std::deque<expr::gsFeVariable<T> > m_vlist;
    std::deque<expr::gsFeSpace<T> >    m_slist;

    // background functions
    FunctionTable m_itable;
    FunctionTable m_ptable;
    //FunctionTable i_map;

    // geometry map
    expr::gsGeometryMap<T> mapVar;
public:
    gsMapData<T> mapData;
private:

    // mutable pair of variable and data,
    // ie. not uniquely assigned to a gsFunctionSet
    expr::gsFeVariable<T> mutVar ;
    gsFuncData<T>         mutData;
    bool mutParametric;

    gsSortedVector<const gsFunctionSet<T>*> evList;

    const gsMultiBasis<T> * mesh_ptr;

public:
    typedef const expr::gsGeometryMap<T> & geometryMap;
    typedef const expr::gsFeElement<T>   & element;
    typedef const expr::gsFeVariable<T>  & variable;
    typedef const expr::gsFeSpace<T>     & space;
    typedef const expr::gsNullExpr<T>      nullExpr;

    
    typedef expr::gsFeVariable<T>  & nonConstVariable;
    typedef expr::gsFeSpace<T>     & nonConstSpace;

    typedef memory::unique_ptr<gsExprHelper> uPtr;
    typedef memory::shared_ptr<gsExprHelper>  Ptr;
public:

    gsMatrix<T> & points() { return mapData.points; }

    static uPtr make() { return uPtr(new gsExprHelper()); }

    void reset()
    {
        m_ptable.clear();
        m_itable.clear();
        points().clear();
        m_vlist .clear();
        m_slist .clear();
        //mapVar.reset();
    }

    void cleanUp()
    {
        mapData.clear();
        mutData.clear();
        for (ftIterator it = m_ptable.begin(); it != m_ptable.end(); ++it)
            it->second.clear();
        for (ftIterator it = m_itable.begin(); it != m_itable.end(); ++it)
            it->second.clear();
    }

    void setMultiBasis(const gsMultiBasis<T> & mesh) { mesh_ptr = &mesh; }

    bool multiBasisSet() { return NULL!=mesh_ptr;}

    const gsMultiBasis<T> & multiBasis()
    {
        GISMO_ASSERT(multiBasisSet(), "Integration elements not set.");
        return *mesh_ptr;
    }


    geometryMap getMap(const gsFunction<T> & mp)
    {
        //mapData.clear();
        mapVar.registerData(mp, mapData);
        return mapVar;
    }

    geometryMap getMap(const gsMultiPatch<T> & mp)
    {
        //mapData.clear();
        mapVar.registerData(mp, mapData);
        return mapVar;
    }

    // /*
    geometryMap getMap() const
    {
        //assert initialized
        return mapVar;
    }
    //*/

    nonConstVariable getVar(const gsFunctionSet<T> & mp, index_t dim = 1)
    {
        m_vlist.push_back( expr::gsFeVariable<T>() );
        expr::gsFeVariable<T> & var = m_vlist.back();
        gsFuncData<T> & fd = m_ptable[&mp];
        fd.dim = mp.dimensions();
        var.registerData(mp, fd, dim);
        return var;
    }

    nonConstVariable getVar(const gsFunctionSet<T> & mp, geometryMap G)
    {
        GISMO_ASSERT(&G==&mapVar, "geometry map not known");
        m_vlist.push_back( expr::gsFeVariable<T>() );
        expr::gsFeVariable<T> & var = m_vlist.back();
        gsFuncData<T> & fd = m_itable[&mp];
        fd.dim = mp.dimensions();
        var.registerData(mp, fd, 1, mapData);
        return var;
    }

    nonConstSpace getSpace(const gsFunctionSet<T> & mp, index_t dim = 1)
    {
        m_slist.push_back( expr::gsFeSpace<T>() );
        expr::gsFeSpace<T> & var = m_slist.back();
        gsFuncData<T> & fd = m_ptable[&mp];
        fd.dim = mp.dimensions();
        var.registerData(mp, fd, dim);
        return var;
    }

    //void rmVar(

    bool exists(variable a)
    {
        typedef typename std::deque<expr::gsFeSpace<T> >::const_iterator siter;
        for (siter it = m_slist.begin(); it!=m_slist.end(); ++it)
            if ( &a == &(*it) ) return true;

        typedef typename std::deque<expr::gsFeVariable<T> >::const_iterator viter;
        for (viter it = m_vlist.begin(); it!=m_vlist.end(); ++it)
            if ( &a == &(*it) ) return true;

        return false;
    }

    variable getMutVar() const { return mutVar; }

    void setMutSource(const gsFunction<T> & func, bool param)
    {
        mutVar.setSource(func);
        mutParametric = param;
    }

    template<class E>
    void check(const expr::_expr<E> & testExpr) const
    {
        if ( testExpr.isVector() )
            GISMO_ENSURE(m_ptable.find(&testExpr.rowVar().source())!=m_ptable.end(), "Check failed");
        if ( testExpr.isMatrix() )
            GISMO_ENSURE(m_ptable.find(&testExpr.colVar().source())!=m_ptable.end(), "Check failed");

        // todo: varlist ?
    }

    void initFlags(const unsigned fflag = 0,
                   const unsigned mflag = 0)
    {
        mapData.flags = mflag;
        mutData.flags = fflag;
        for (ftIterator it = m_ptable.begin(); it != m_ptable.end(); ++it)
            it->second.flags = fflag;
        for (ftIterator it = m_itable.begin(); it != m_itable.end(); ++it)
            it->second.flags = fflag;
    }

    template<class Expr> // to remove
    void setFlags(const Expr & testExpr,
                  const unsigned fflag = 0,
                  const unsigned mflag = 0)
    {
        // todo:
        //testExpr.variables_into(m_ptable);
        // plus auto-registration

        initFlags(fflag, mflag);
        testExpr.setFlag(); // protected ?
    }

    //void precompute(const gsMatrix<T> & points, const index_t patchIndex = 0)

    void precompute(const index_t patchIndex = 0)
    {
        GISMO_ASSERT(0!=points().size(), "No points");

        //mapData.side
        if ( mapVar.isValid() ) // list ?
        {
            //mapData.flags |= NEED_VALUE;
            mapVar.source().function(patchIndex).computeMap(mapData);
            mapData.patchId = patchIndex;
        }
        
        if ( mutVar.isValid() )
        {
            GISMO_ASSERT( mutParametric || 0!=mapData.values.size(), "Map values not computed");
            //mutVar.source().piece(patchIndex).compute(mapData.points, mutData);
            mutVar.source().piece(patchIndex)
                .compute( mutParametric ? mapData.points : mapData.values[0], mutData);
        }

        for (ftIterator it = m_ptable.begin(); it != m_ptable.end(); ++it)
        {
            it->first->piece(patchIndex).compute(mapData.points, it->second); // ! piece(.) ?
            it->second.patchId = patchIndex;
        }

        GISMO_ASSERT( m_itable.empty() || 0!=mapData.values.size(), "Map values not computed");

        for (ftIterator it = m_itable.begin(); it != m_itable.end(); ++it)
        {
            it->first->piece(patchIndex).compute(mapData.values[0], it->second);
            it->second.patchId = patchIndex;
        }
    }

    template<class E1, class E2>
    void parse(const expr::_expr<E1> & expr1, const expr::_expr<E2> & expr2)
    {
        //evList.reserve(m_ptable.size()+m_itable.size());
        evList.clear();
        expr1.parse(evList);
        expr2.parse(evList);
    };

/*
    void precompute(const index_t patch1, const index_t patch2)
    {
        GISMO_ASSERT(0!=points().size(), "No points");

        //mapData.side
        if ( mapVar.isValid() ) // list ?
            mapVar.source().function(patchIndex).computeMap(mapData);

        if ( mutVar.isValid() )
            //mutVar.source().piece(patchIndex).compute(mapData.points, mutData);
            mutVar.source().piece(patchIndex)
                .compute( mutParametric ? mapData.points : mapData.values[0], mutData);

        for (ftIterator it = m_ptable.begin(); it != m_ptable.end(); ++it)
            it->first->piece(patchIndex).compute(mapData.points, it->second); // ! piece(.) ?

        for (ftIterator it = m_itable.begin(); it != m_itable.end(); ++it)
            it->first->piece(patchIndex).compute(mapData.values[0], it->second);
    }
//*/


};//class


} //namespace gismo