#ifndef _LOGINQUERYHOOLDER_H_
#define _LOGINQUERYHOOLDER_H_

#include "DatabaseEnv.h"
#include "SQLOperation.h"
#include "MySQLPreparedStatement.h"
#include "PreparedStatement.h"

#include "DatabaseEnvFwd.h"
#include "Implementation/LoginDatabase.h"
#include "Implementation/WorldDatabase.h"
#include "Implementation/CharacterDatabase.h"
#include "Implementation/HotfixDatabase.h"
#include "QueryHolder.h"
#include "SQLOperation.h"
#include "DatabaseEnvFwd.h"
#include "World.h"

class QueryHolder;

/*
class LoginQueryHolder //: public LoginDatabaseQueryHolder //: public SQLQueryHolderCallback
{
    uint32 m_accountId;
    ObjectGuid m_guid;
public:
    // LoginQueryHolder(uint32 accountId, ObjectGuid guid);
    ObjectGuid const& GetGuid() { return m_guid; };
    uint32 GetAccountId() { return m_accountId; };
    bool Initialize(LoginDatabaseConnection* temp);
    bool Initialize(CharacterDatabaseConnection* temp);

    PreparedQueryResult::GetPreparedResult(size_t index)
{
    // Don't call to this function if the index is of a prepared statement
    if (index < m_queries.size())
    {
        PreparedResultSet* result = m_queries[index].second.presult;
        if (!result || !result->GetRowCount())
            return PreparedQueryResult(NULL);

        return PreparedQueryResult(result);
    }
    else
        return PreparedQueryResult(NULL);
}
    LoginQueryHolder(const uint32& m_accountId, const ObjectGuid& m_guid)
        : m_accountId(m_accountId), m_guid(m_guid)
    {
    }
};
*/
class LoginQueryHolder : public LoginDatabaseQueryHolder
{
private:
    uint32 m_accountId;
    ObjectGuid m_guid;
public:
    static LoginQueryHolder* instance()
    {
        static LoginQueryHolder* instance;
        return instance;
    }
    LoginQueryHolder(uint32 accountId, ObjectGuid guid)
        : m_accountId(accountId), m_guid(guid) { }

    ~LoginQueryHolder()
    {
        delete instance();
    };

    ObjectGuid GetGuid() const { return m_guid; }
    uint32 GetAccountId() const { return m_accountId; }
    bool Initialize();
};

class CharacterQueryHolder : public CharacterDatabaseQueryHolder
{
private:
    uint32 m_accountId;
    ObjectGuid m_guid;
public:
    static CharacterQueryHolder* instance()
    {
        static CharacterQueryHolder* instance;
        return instance;
    }
    CharacterQueryHolder(uint32 accountId, ObjectGuid guid)
        : m_accountId(accountId), m_guid(guid) { }

    ~CharacterQueryHolder()
    {
        delete instance();
    };

    ObjectGuid GetGuid() const { return m_guid; }
    uint32 GetAccountId() const { return m_accountId; }
    bool Initialize();
};

class EnumCharactersQueryHolder : public CharacterDatabaseQueryHolder
{
public:
    enum
    {
        CHARACTERS,
        CUSTOMIZATIONS,

        MAX
    };

    EnumCharactersQueryHolder()
    {
        SetSize(MAX);
    }

    bool Initialize(uint32 accountId, bool withDeclinedNames, bool isDeletedCharacters)
    {
        _isDeletedCharacters = isDeletedCharacters;

        constexpr CharacterDatabaseStatements statements[2][3] =
        {
            { CHAR_SEL_ENUM, CHAR_SEL_ENUM_DECLINED_NAME, CHAR_SEL_ENUM_CUSTOMIZATIONS },
            { CHAR_SEL_UNDELETE_ENUM, CHAR_SEL_UNDELETE_ENUM_DECLINED_NAME, CHAR_SEL_UNDELETE_ENUM_CUSTOMIZATIONS }
        };

        bool result = true;
        CharacterDatabasePreparedStatement* stmt = CharacterDatabase.GetPreparedStatement(statements[isDeletedCharacters ? 1 : 0][withDeclinedNames ? 1 : 0]);
        stmt->setUInt32(0, accountId);
        result &= SetPreparedQuery(CHARACTERS, stmt);

        stmt = CharacterDatabase.GetPreparedStatement(statements[isDeletedCharacters ? 1 : 0][2]);
        stmt->setUInt32(0, accountId);
        result &= SetPreparedQuery(CUSTOMIZATIONS, stmt);

        return result;
    }

    bool IsDeletedCharacters() const { return _isDeletedCharacters; }

private:
    bool _isDeletedCharacters = false;
};

/*
class LoginInfoQueryHolder // : public LoginInfoQueryHolder //: public SQLQueryHolderCallback
{
    uint32 m_accountId;
    ObjectGuid m_guid;
public:
    // LoginQueryHolder(uint32 accountId, ObjectGuid guid);
    ObjectGuid const& GetGuid() { return m_guid; };
    uint32 GetAccountId() { return m_accountId; };
    bool Initialize(CharacterInfoQueryHolder::SQLQueryHolderBase);

    LoginInfoQueryHolder(const uint32& m_accountId, const ObjectGuid& m_guid)
        : m_accountId(m_accountId), m_guid(m_guid)
    {
    }
};
*/
/*
class LoginQueryHolder // : public LoginbaseQueryHolder // SQLQueryHolderBase
{
    uint32 m_accountId;
    ObjectGuid m_guid;
public:
    LoginQueryHolder(uint32 accountId, ObjectGuid guid);
    ObjectGuid const& GetGuid() const;
    uint32 GetAccountId() const;
    bool Initialize();
    LoginQueryHolder(const uint32& m_accountId, const ObjectGuid& m_guid)
        : m_accountId(m_accountId), m_guid(m_guid) {};
}; */

#endif
