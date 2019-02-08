#ifndef _MYSQLCONNECTION_H
#define _MYSQLCONNECTION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DatabaseEnvFwd.h"
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

template <typename T>
class ProducerConsumerQueue;

enum ConnectionFlags
{
    CONNECTION_ASYNC = 0x1,
    CONNECTION_SYNCH = 0x2,
    CONNECTION_BOTH = CONNECTION_ASYNC | CONNECTION_SYNCH
};

struct  MySQLConnectionInfo
{
    explicit MySQLConnectionInfo(std::string const& infoString);

    std::string user;
    std::string password;
    std::string database;
    std::string host;
    std::string port_or_socket;
};

class  MySQLConnection
{
    template <class T> friend class DatabaseWorkerPool;
    friend class PingOperation;

    public:
        MySQLConnection(MySQLConnectionInfo& connInfo);                               //! Constructor for synchronous connections.
        MySQLConnection(ProducerConsumerQueue<SQLOperation*>* queue, MySQLConnectionInfo& connInfo);  //! Constructor for asynchronous connections.
        virtual ~MySQLConnection();

        virtual uint32 Open();
        void Close();

        bool PrepareStatements();

    public:
        bool Execute(char const* sql);
        bool Execute(PreparedStatement* stmt);
        ResultSet* Query(char const* sql);
        PreparedResultSet* Query(PreparedStatement* stmt);
        bool _Query(char const* sql, MYSQL_RES** pResult, MYSQL_FIELD** pFields, uint64* pRowCount, uint32* pFieldCount);
        bool _Query(PreparedStatement* stmt, MYSQL_RES** pResult, uint64* pRowCount, uint32* pFieldCount);

        void BeginTransaction();
        void RollbackTransaction();
        void CommitTransaction();
        int ExecuteTransaction(SQLTransaction& transaction);

        void Ping();

        uint32 GetLastError();

    protected:
        /// Tries to acquire lock. If lock is acquired by another thread
        /// the calling parent will just try another connection
        bool LockIfReady();

        /// Called by parent databasepool. Will let other threads access this connection
        void Unlock();

        MYSQL* GetHandle()  { return m_Mysql; }
        MySQLPreparedStatement* GetPreparedStatement(uint32 index);
        void PrepareStatement(uint32 index, std::string const& sql, ConnectionFlags flags);

        virtual void DoPrepareStatements() = 0;

    protected:
        typedef std::vector<std::unique_ptr<MySQLPreparedStatement>> PreparedStatementContainer;

        PreparedStatementContainer           m_stmts;         //! PreparedStatements storage
        bool                                 m_reconnecting;  //! Are we reconnecting?
        bool                                 m_prepareError;  //! Was there any error while preparing statements?

    private:
        bool _HandleMySQLErrno(uint32 errNo, uint8 attempts = 5);

    private:
        ProducerConsumerQueue<SQLOperation*>* m_queue;      //! Queue shared with other asynchronous connections.
        std::unique_ptr<DatabaseWorker> m_worker;           //! Core worker task.
        MYSQL*                m_Mysql;                      //! MySQL Handle.
        MySQLConnectionInfo&  m_connectionInfo;             //! Connection info (used for logging)
        ConnectionFlags       m_connectionFlags;            //! Connection flags (for preparing relevant statements)
        std::mutex            m_Mutex;

        MySQLConnection(MySQLConnection const& right) = delete;
        MySQLConnection& operator=(MySQLConnection const& right) = delete;
};

#endif
