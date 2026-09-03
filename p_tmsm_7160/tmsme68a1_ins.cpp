/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-24 14:14:40
Description: 扇形段通钢量
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme68a1_ins)


int f_tmsme68a1_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm68("TTMSM68");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);
	CDataTable dt_dev;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm68.Reset();
			ttmsm68.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm68.TrimOrBlank();

			/* 检查输入参数合法性 */
			if (ttmsm68["STRAND_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "流号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm68["CC_MACH_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "铸机号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			Log::Trace("", __FUNCTION__, "ttmsm68.CC_MACH_NO		= [{0}]", (const char*)ttmsm68["CC_MACH_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm68.STRAND_NO		= [{0}]", (const char*)ttmsm68["STRAND_NO"].ToString());



			/* 查询该主设备类型是否存在 */
			//if (ttmsm68.QueryCount("HEAT_NO") > 0)
			//{
			//	sprintf(s.msg, "熔炼号信息[%s]已存在", (const char*)ttmsm69["熔炼号"].ToString());
			//	throw CApplicationException(-1, s.msg, s.svc_name);
			//}
				
			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='CC_MACH_NO' AND CODE = @mach_no";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("mach_no", ttmsm68["CC_MACH_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["REMARK_3"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["REMARK_3"] = " ";
			}
			cmd_inq.Close();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='STRAND_NO' AND CODE = @strand_no";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("strand_no", ttmsm68["STRAND_NO"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["REMARK_4"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["REMARK_4"] = " ";
			}
			cmd_inq.Close();

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='STATUS' AND CODE = @status_no";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("status_no", ttmsm68["STATUS"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["REMARK_5"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["REMARK_5"] = " ";
			}
			cmd_inq.Close();


			//ttmsm66["CHANGE_TIME"] = datetime;		    //操作时刻
			ttmsm68["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm68["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm68["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm68["REC_REVISE_TIME"] = datetime;		//记录修改时刻

			ttmsm68["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_68", conn));


			Log::Trace("", __FUNCTION__, "状态		= [{0}]", (const char*)ttmsm68["STATUS"].ToString());

			ttmsm68.TrimOrBlank();
			ttmsm68.Insert();


		}

	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		//返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		//数据库异常时返回-1，事务将被回滚
		doFlag = -1;
	}
	//捕获应用错误
	catch (CApplicationException& ex)
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

	cmd_inq.Close();
	//返回-1时事务将回滚，返回为0是事务将提交
	return doFlag;
}


