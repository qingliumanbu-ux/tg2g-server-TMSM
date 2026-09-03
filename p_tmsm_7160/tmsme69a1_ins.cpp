/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-23 18:48:02
Description: RH真空槽信息新增
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme69a1_ins)


int f_tmsme69a1_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm69("TTMSM69");

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
			ttmsm69.Reset();
			ttmsm69.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm69.TrimOrBlank();

			/* 检查输入参数合法性 */
			if (ttmsm69["HEAT_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "熔炼号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
		
			Log::Trace("", __FUNCTION__, "ttmsm69.HEAT_NO		= [{0}]", (const char*)ttmsm69["HEAT_NO"].ToString());



			/* 查询该主设备类型是否存在 */
			if (ttmsm69.QueryCount("HEAT_NO") > 0)
			{
				sprintf(s.msg, "熔炼号信息[%s]已存在", (const char*)ttmsm69["熔炼号"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//ttmsm66["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));

			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	    // Oracle 数据库
			//default:
			//	sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME66' AND CODE_CLASS='CHANGE_RES_C' AND CODE = @change_res";
			//	break;
			//}
			//cmd_inq.Parameters.Clear();
			//cmd_inq.Parameters.Set("change_res", ttmsm66["CHANGE_RES"].ToString());
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteReader();

			//if (cmd_inq.Read())
			//{
			//	ttmsm66["CHANGE_RES_C"] = cmd_inq.GetString(1);
			//}
			//else
			//{
			//	ttmsm66["CHANGE_RES_C"] = " ";
			//}
			//cmd_inq.Close();


			//ttmsm66["CHANGE_TIME"] = datetime;		    //操作时刻
			ttmsm69["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm69["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm69["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm69["REC_REVISE_TIME"] = datetime;		//记录修改时刻

			Log::Trace("", __FUNCTION__, "喷补量		= [{0}]", (const char*)ttmsm69["SPURT_MAT_WT"].ToString());

			ttmsm69.TrimOrBlank();
			ttmsm69.Insert();


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


