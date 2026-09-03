/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:   
Version:    1.0
Date:     2014-01-26
Description: 铁水包实绩获取
**************************************************/
#include "stdafx.h"


/*<remark>=========================================================
/// <summary>
/// 炼钢工器具流水号生成
/// <para>
/// 1.根据传入的流水号名称获取最新号值
/// </para>
/// <returns>配包信息</returns>
===========================================================</remark>*/


int f_tmsm_getnextseq(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	// 程序内部变量
	int doFlag = 0;
	int blknum = 0;

	// 实体类定义

	// 数据库SQL操作字符串
	CString sqlstr = "";

	// 数据库操作类定义
	CDbCommand cmd(conn);

	// 业务变量
	CString	seq_name = ""; //序号名
	CDecimal seq_val = -1; //序号值

	CString sm_unit_no = "";


	try
	{
		//定义返回结构
		blknum = 0;
		if ( !bcls_ret->Tables.Contains("RETSEQ") )
			bcls_ret->Tables.Add("RETSEQ");
		if (!bcls_ret->Tables["RETSEQ"].Columns.Contains("SEQ_VAL"))
			bcls_ret->Tables["RETSEQ"].Columns.Add(DT_DECIMAL, "SEQ_VAL");   //返回值
		if (bcls_ret->Tables["RETSEQ"].Rows.get_Count() == 0)  //没有记录
			bcls_ret->Tables["RETSEQ"].Rows.Add();

		//--------------------------------
		//获取传入参数
		blknum = bcls_rec->Tables.IndexOf("GETSEQ");
		if (blknum < 0) //没有该块
		{
			strcpy(s.msg, "传入数据块[GETSEQ]不存在，请联系系统维护人员。");
			sprintf(s.sysmsg, "TABLE [GETSEQ] NOT EXIST.");
			throw CApplicationException(-1, s.msg, log.Location);
		}

		//获取Sequence名，单记录
		seq_name = bcls_rec->Tables[blknum].Rows[0]["SEQ_NAME"].ToString().Trim();

		Log::Info(" ", __FUNCTION__, "seq_name =[{0}]", seq_name);


		//--------------------------------
		//1.判断序列名是否存在，不存在，自动创建
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:        // Oracle 数据库
			sqlstr = CString("SELECT count(*) FROM user_sequences t where t.sequence_name = '" + seq_name.Trim() + "' ");
		default: // 所有数据库适用，通用SQL语句
			break;
		}
		cmd.SetCommandText(sqlstr);
		CDecimal cnt = cmd.ExecuteScalar();

		Log::Trace(" ", __FUNCTION__, "exist =[{0}]", cnt);
		if (cnt == 0) //不存在
		{
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:        // Oracle 数据库
			default: // 所有数据库适用，通用SQL语句
				sqlstr = CString("create sequence " + seq_name.Trim() +
					" minvalue 0 "
					" maxvalue 9999999 "
					" start with 1 "
					" increment by 1 "
					" cycle "
					);
				break;
			}
			cmd.SetCommandText(sqlstr);
			int created = cmd.ExecuteNonQuery();
		}


		//根据指定序号名，获取序号值
		switch (conn->DatabaseKind)
		{
		case DB_KIND_DB2:           // DB2 数据库（未开Oracle兼容）
		case DB_KIND_DB2_ORACLE:    // DB2 数据库（开Oracle兼容）
		case DB_KIND_MSSQL:	        // MS SQL Server数据库
		case DB_KIND_ORACLE:        // Oracle 数据库
		default: // 所有数据库适用，通用SQL语句
			sqlstr = CString("select " + seq_name.Trim() + ".nextval from dual ");
			break;
		}
		cmd.SetCommandText(sqlstr);
		cmd.ExecuteReader();
		if (cmd.Read())
		{
			seq_val = cmd.GetDecimal(1);
		}
		else
		{
			seq_val = -1;
		}

		Log::Debug(" ", __FUNCTION__, "seq_val =[{0}]", seq_val);

		//块返回顺序值
		bcls_ret->Tables["RETSEQ"].Rows[0]["SEQ_VAL"] = seq_val;

		//函数返回
		doFlag = seq_val.ToInt32();


	}
	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (const CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg) - 1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	return doFlag;
}
