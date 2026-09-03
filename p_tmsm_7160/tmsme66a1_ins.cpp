/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-22 15:55:56
Description: 扇形段跟踪信息新增
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme66a1_ins)


int f_tmsme66a1_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm66("TTMSM66");
	CModel ttmsm67("TTMSM67");

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
			ttmsm66.Reset();
			ttmsm66.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm66.TrimOrBlank();

			/* 检查输入参数合法性 */
			if (ttmsm66["SM_UNIT_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "炼钢单元号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm66["CC_MACH_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "连铸机号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm66["STRAND_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "流号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm66["DEV_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "设备编码不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}



			Log::Trace("", __FUNCTION__, "ttmsm66.STRAND_NO		= [{0}]", (const char*)ttmsm66["STRAND_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.CC_MACH_NO		= [{0}]", (const char*)ttmsm66["CC_MACH_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.DEV_NO		= [{0}]", (const char*)ttmsm66["DEV_NO"].ToString());



			/* 查询该主设备类型是否存在 */
			//if (ttmsm61.QueryCount("OP_DIV,SM_UNIT_NO,IRON_LADLE_NO") > 0)
			if (ttmsm66.QueryCount("SM_UNIT_NO,CC_MACH_NO,STRAND_NO,DEV_NO") > 0)
			{
				sprintf(s.msg, "扇形段跟踪信息[%s]已存在", (const char*)ttmsm66["DEV_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//ttmsm66["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));

			if (ttmsm66["DEV_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "设备编码不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME66' AND CODE_CLASS='CHANGE_RES_C' AND CODE = @change_res"	;
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("change_res", ttmsm66["CHANGE_RES"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm66["CHANGE_RES_C"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm66["CHANGE_RES_C"] = " ";
			}
			cmd_inq.Close();


			//ttmsm66["CHANGE_TIME"] = datetime;		    //操作时刻
			ttmsm66["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm66["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm66["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm66["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			ttmsm66["CHANGE_TIME"] = datetime;		    //变动时刻

			Log::Trace("", __FUNCTION__, "ttmsm66.CHANGE_RES		= [{0}]", (const char*)ttmsm66["CHANGE_RES"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.CHANGE_RES_C		= [{0}]", (const char*)ttmsm66["CHANGE_RES_C"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.INSPECT_CONCL_NOZZLE		= [{0}]", (const char*)ttmsm66["INSPECT_CONCL_NOZZLE"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm66.REMARK_2		= [{0}]", (const char*)ttmsm66["REMARK_2"].ToString());

			ttmsm66.TrimOrBlank();
			ttmsm66.Insert();


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


