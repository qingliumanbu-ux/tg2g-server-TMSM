/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-17 13:49:00
Description: 工器具主体信息新增
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme91a1_ins)


int f_tmsme91a1_ins(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	CModel ttmsm91("TTMSM91");
	CModel ttmsm92("TTMSM92");

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
			ttmsm91.Reset();
			ttmsm91.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm91.TrimOrBlank();

			/* 检查输入参数合法性 */
			if (ttmsm91["DEV_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "设备编码不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			if (ttmsm91["DEV_NO"].ToString().Trim().Substring(0, 2) != ttmsm91["DEV_TYPE"].ToString().Trim())
			{
				strcpy(s.msg, "设备编码有问题!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			Log::Trace("", __FUNCTION__, "ttmsm91.DEV_TYPE		= [{0}]", (const char*)ttmsm91["DEV_TYPE"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm91.DEV_NO		= [{0}]", (const char*)ttmsm91["DEV_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm91.DEV_NAME		= [{0}]", (const char*)ttmsm91["DEV_NAME"].ToString());



			/* 查询该主设备类型是否存在 */
			//if (ttmsm61.QueryCount("OP_DIV,SM_UNIT_NO,IRON_LADLE_NO") > 0)
			if (ttmsm91.QueryCount("DEV_NO") > 0)
			{
				sprintf(s.msg, "设备编码[%s]已存在", (const char*)ttmsm91["DEV_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			ttmsm91["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = " 	SELECT WORK_AREA FROM TTMSM61 "
					" WHERE DEV_TYPE = @ttmsm91.DEV_TYPE   "
					;
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("ttmsm91.DEV_TYPE", ttmsm91["DEV_TYPE"].ToString());
			cmd_inq.SetCommandText(sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm91["WORK_AREA"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm91["WORK_AREA"] = " ";
			}			
			cmd_inq.Close();


			ttmsm91["OPERATE_TIME"] = datetime;		    //操作时刻
			ttmsm91["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm91["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm91["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm91["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			ttmsm91.TrimOrBlank();
			ttmsm91.Insert();

			sqlstr = " SELECT * FROM TTMSM62 WHERE DEV_TYPE = '" + ttmsm91["DEV_TYPE"].ToString().Trim() + "'"
				" ORDER BY SECTION_MODE";
			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::QueryTable(sqlstr, dt_dev);
			Log::Trace("", "", "Count：{0}", dt_dev.Rows.get_Count());
			for (int i = 0; i < dt_dev.Rows.get_Count(); i++)
			{
				ttmsm92["SM_UNIT_NO"] = ttmsm91["SM_UNIT_NO"].ToString();
				ttmsm92["DEV_NO"] = ttmsm91["DEV_NO"].ToString();
				ttmsm92["DEV_TYPE"] = ttmsm91["DEV_TYPE"].ToString();

				ttmsm92["SECTION_MODE"] = dt_dev.Rows[i]["SECTION_MODE"].ToString();
				ttmsm92["SECTION_DEFINE"] = dt_dev.Rows[i]["SECTION_DEFINE"].ToString();
				ttmsm92["MOLD_TYPE"] = dt_dev.Rows[i]["MOLD_TYPE"].ToString();
				ttmsm92["OPERATE_TYPE"] =  dt_dev.Rows[i]["OPERATE_TYPE"].ToString();
				ttmsm92["CHANGE_TYPE"] = dt_dev.Rows[i]["CHANGE_TYPE"].ToString();
				
				
				ttmsm92["REC_CREATOR"] = s.userid;			//记录创建责任者
				ttmsm92["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				ttmsm92["REC_REVISOR"] = s.userid;			//记录修改责任者
				ttmsm92["REC_REVISE_TIME"] = datetime;		//记录修改时刻

				ttmsm92.TrimOrBlank();
				ttmsm92.Insert();

			}

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


