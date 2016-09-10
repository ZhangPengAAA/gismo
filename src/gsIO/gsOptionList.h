/** @file gsOptionList.h

    @brief Provides a list of labeled parameters/options that can be
    set and accessed easily

    This file is part of the G+Smo library.
    
    This Source Code Form is subject to the terms of the Mozilla Public
    License, v. 2.0. If a copy of the MPL was not distributed with this
    file, You can obtain one at http://mozilla.org/MPL/2.0/.

    Author(s): A. Mantzaflaris, H. Weiner
*/

#pragma once

#include <gsCore/gsForwardDeclarations.h>
#include <gsIO/gsXml.h>

namespace gismo
{

/** 
    @brief Class which holds a list of parameters/options, and
    provides easy access to them.
    
    Every parameter has a unique label, and its value can be retrieved
    using that.
    
    \ingroup IO
*/
class GISMO_EXPORT gsOptionList
{
public:
    
    /// \brief Reads value for option \a label from options. If \a
    /// label is not found, the function throws
    std::string getString(const std::string & label) const;
    /// @copydoc gsOptionList::getString
    int         getInt   (const std::string & label) const;
    /// @copydoc gsOptionList::getString
    real_t      getReal  (const std::string & label) const;
    /// @copydoc gsOptionList::getString
    bool        getSwitch(const std::string & label) const;

    /// \brief Reads value for option \a label from options. If \a
    /// label is not found, it defaults to \a val (otherwise \a val is
    /// not used)
    std::string askString(const std::string & label, const std::string& val = "") const;
    /// @copydoc gsOptionList::getString
    int         askInt   (const std::string & label, const int &        val = 0 ) const;
    /// @copydoc gsOptionList::getString
    real_t      askReal  (const std::string & label, const real_t &     val = 0 ) const;
    /// @copydoc gsOptionList::getString
    bool        askSwitch(const std::string & label, const bool &   val = false ) const;

    /// \brief Sets an existing option \a label to be equal to \a
    /// value
    void setString(const std::string & label, const std::string & value);
    /// @copydoc gsOptionList::setString
    void setInt   (const std::string & label, const int & res          );
    /// @copydoc gsOptionList::setString
    void setReal  (const std::string & label, const real_t & res       );
    /// @copydoc gsOptionList::setString
    void setSwitch(const std::string & label, const bool & res         );

    /// \brief Adds a new option named \a label, with description \a
    /// desc and current value \a value
    void addString(const std::string& label, const std::string& desc, const std::string& value);
    /// @copydoc gsOptionList::addString
    void addInt   (const std::string& label, const std::string& desc, const int& value);
    /// @copydoc gsOptionList::addString
    void addReal  (const std::string& label, const std::string& desc, const real_t& value);
    /// @copydoc gsOptionList::addString
    void addSwitch(const std::string& label, const std::string& desc, const bool& value);

    /// \brief Prints this list of options to stream \a os
    std::ostream & print(std::ostream & os) const;

    /// \brief Returns the length of this list of options
    int size() const
    {return m_strings.size()+m_ints.size()+m_reals.size()+m_switches.size();}

    // /*
    typedef struct {
    	std::string type;
    	std::string label;
    	std::string desc;
    	std::string val;
    } OptionListEntry;
    
    std::vector<OptionListEntry> getAllEntries() const;
    //*/

    gsOptionList& operator=(const gsOptionList& other)
    {
        if (this != &other)
        {
            m_strings  = other.m_strings;
            m_ints     = other.m_ints;
            m_reals    = other.m_reals;
            m_switches = other.m_switches;            
        }
        return *this;
    }

private:

    /// \brief Prints information regarding the option nnamed \a label
    void printInfo(const std::string & label) const;

    /// \brief Returns true iff an option named \a label exists
    bool exists(const std::string & label) const;
    
private:
    friend class internal::gsXml<gsOptionList>;
    
    // Format: std::pair<Value,Description>
    typedef std::pair<std::string,std::string> StringOpt;
    typedef std::pair<int        ,std::string> IntOpt;
    typedef std::pair<real_t     ,std::string> RealOpt;
    typedef std::pair<bool       ,std::string> SwitchOpt;

    // Format: std::map<Label, std::pair<Value,Description> >
    typedef std::map<std::string,StringOpt> StringTable;
    typedef std::map<std::string,IntOpt>    IntTable;
    typedef std::map<std::string,RealOpt>   RealTable;
    typedef std::map<std::string,SwitchOpt> SwitchTable;
    
    StringTable m_strings;  ///< String-valued options/parameters
    IntTable    m_ints;     ///< Integer-valued options/parameters
    RealTable   m_reals;    ///< Real-valued options/parameters
    SwitchTable m_switches; ///< Switches (ON/OFF) options/parameters

}; // class gsOptionList

/// Print (as string) operator to be used by all derived classes
inline std::ostream &operator<<(std::ostream &os, const gsOptionList& b)
{return b.print(os); }


namespace internal
{

/** \brief Read OptionList from XML data
    \ingroup IO
*/
template<>
class gsXml<gsOptionList>
{
private:
    gsXml() { }

public:
    GSXML_COMMON_FUNCTIONS(gsOptionList)
    GSXML_GET_POINTER(gsOptionList)
    static std::string tag () { return "OptionList"; }
    static std::string type() { return ""; }

    static void get_into(gsXmlNode * node, gsOptionList & result)
    {
    	// get all child-nodes
    	gsXmlNode * tmp = node->first_node();
    	while ( tmp )
        {
    		const char* name = tmp->name();
    		//gsDebug << "\nFound child node with name='" << name << "'\n";

    		const std::string label = tmp->first_attribute("label")->value();
    		const std::string desc = tmp->first_attribute("desc")->value();
    		const std::string val = tmp->first_attribute("value")->value();

    		if (strcmp("int", name) == 0)
            {
    			std::istringstream str;
    			str.str( val );
    			int myVal;
    			gsGetInt(str, myVal);
    			result.addInt(label, desc, myVal);
    			//gsDebug << "\nresult.addInt(label'" << label << "',desc='";
    			//gsDebug<< desc << "','val='" << myVal << "')\n";
    			//gsDebug << "result.getInt(label='" << label << "')='";
    			//gsDebug<< result.getInt(label) << "'\n";
    		}
    		else if (strcmp("real", name) == 0)
            {
    			std::istringstream str;
    			str.str( val );
    			real_t myVal;
    			gsGetReal(str, myVal);
    			result.addReal(label, desc, myVal);
    			//gsDebug << "\nresult.addReal(label'" << label << "',desc='";
    			//gsDebug << desc << "','val='" << myVal << "')\n";
    		}
    		else if (strcmp("bool", name) == 0)
            {
    			std::istringstream str;
    			str.str( val );
    			int myVal;
    			gsGetInt(str, myVal);
    			bool myBoolVal = (myVal != 0);
    			result.addSwitch(label, desc, myBoolVal);
    			//gsDebug << "\nresult.addSwitch(label'" << label << "',desc='";
    			//gsDebug << desc << "','val='" << myBoolVal << "')\n";
    		}
    		else
            {
    			//gsDebug << "\nresult.addString(label'" << label << "',desc='";
    			//gsDebug << desc << "','val='" << val << "')\n";
    			result.addString(label, desc, val);
    		}
    		tmp =  tmp->next_sibling();
    	}
    }

    static gsXmlNode * put (const gsOptionList & obj, gsXmlTree & data)
    {
    	// Append data
        gsXmlNode * optionList = internal::makeNode("OptionList", data);

        // /*
        // iterate over all strings
        std::vector<gsOptionList::OptionListEntry> entries = obj.getAllEntries();
        std::vector<gsOptionList::OptionListEntry>::const_iterator it;
        for (it = entries.begin(); it != entries.end(); it++)
        {
        	const gsOptionList::OptionListEntry & entry = *it;
        	gsXmlNode * node_str = internal::makeNode(entry.type, data);
        	gsXmlAttribute * attr_label = internal::makeAttribute("label", entry.label, data);
        	gsXmlAttribute * attr_desc = internal::makeAttribute("desc", entry.desc, data);
        	gsXmlAttribute * attr_val = internal::makeAttribute("value", entry.val, data);
        	node_str->insert_attribute(0, attr_label);
        	node_str->insert_attribute(0, attr_desc);
        	node_str->insert_attribute(0, attr_val);
        	optionList->insert_node(0, node_str);
        }
        // */
        
        /*
        gsXmlNode * tmp;
        gsXmlAttribute * atr;
        gsOptionList::StringTable::const_iterator it1;
        for ( it1 = obj.m_strings.begin(); it1 != obj.m_strings.end(); it1++ )
        {
            tmp = internal::makeNode("string", data);
        	atr = internal::makeAttribute("label", it1->first, data);
            tmp->insert_attribute(0, atr);
        	atr = internal::makeAttribute("desc", it1->second.second, data);
            tmp->insert_attribute(0, atr);
            atr = internal::makeAttribute("value", it1->second.first, data);
            tmp->insert_attribute(0, atr);
            optionList->insert_node(0, tmp);
        }
        gsOptionList::IntTable::const_iterator it2;
        for ( it2 = obj.m_ints.begin(); it2 != obj.m_ints.end(); it2++ )
        {
            tmp = internal::makeNode("int", data);
        	atr = internal::makeAttribute("label", it2->first, data);
            tmp->insert_attribute(0, atr);
        	atr = internal::makeAttribute("desc", it2->second.second, data);
            tmp->insert_attribute(0, atr);
            atr = internal::makeAttribute("value", it2->second.first, data);
            tmp->insert_attribute(0, atr);
            optionList->insert_node(0, tmp);
        }
        gsOptionList::RealTable::const_iterator it3;
        for ( it3 = obj.m_reals.begin(); it3 != obj.m_reals.end(); it3++ )
        {
            tmp = internal::makeNode("real", data);
        	atr = internal::makeAttribute("label", it3->first, data);
            tmp->insert_attribute(0, atr);
        	atr = internal::makeAttribute("desc", it3->second.second, data);
            tmp->insert_attribute(0, atr);
            atr = internal::makeAttribute("value", it3->second.first, data);
            tmp->insert_attribute(0, atr);
            optionList->insert_node(0, tmp);
        }
        gsOptionList::SwitchTable::const_iterator it4;
        for ( it4 = obj.m_switches.begin(); it4 != obj.m_switches.end(); it4++ )
        {
            tmp = internal::makeNode("switch", data);
        	atr = internal::makeAttribute("label", it4->first, data);
            tmp->insert_attribute(0, atr);
        	atr = internal::makeAttribute("desc", it4->second.second, data);
            tmp->insert_attribute(0, atr);
            atr = internal::makeAttribute("value", it4->second.first, data);
            tmp->insert_attribute(0, atr);
            optionList->insert_node(0, tmp);
        }
        */
        
        return optionList;
    }
};

} // namespace internal

} // namespace gismo