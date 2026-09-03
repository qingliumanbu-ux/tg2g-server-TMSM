/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-24 15:35:23
Description: 扇形段通钢量设备下线
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme68a1_offline)


int f_tmsme68a1_offline(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	//CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm68("TTMSM68");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");


		/* 获得传入参数 */
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm68.Reset();
			ttmsm68.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm68.TrimOrBlank();

			//Log::Trace("", __FUNCTION__, "ttmsm68.HEAT_NO		= [{0}]", (const char*)ttmsm69["HEAT_NO"].ToString());

			if (ttmsm68["DEV_NO"].ToString().Trim().GetLength() == 0)
			{
				strcpy(s.msg, "设备编号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
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

			/* 查询该设备是否存在 */
			if (ttmsm68.QueryCount("CC_MACH_NO,STRAND_NO,DEV_NO") <= 0)
			{
				sprintf(s.msg, "下线设备[%s]不存在!", (const char*)ttmsm68["DEV_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			switch (conn->DatabaseKind)
			{
			case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	        // MS SQL Server数据库
			case DB_KIND_ORACLE:	    // Oracle 数据库
			default:
				sqlstr = "select DAYS(DATE(TIMESTAMP ('" + ttmsm68["OFF_LINE_TIME"].ToString() + "'))) - DAYS(DATE(TIMESTAMP ('" + ttmsm68["ON_LINE_TIME"].ToString() + "'))) from SYSIBM.SYSDUMMY1";
				break;
			}
			cmd_inq.Parameters.Clear();
			cmd_inq.SetCommandText(sqlstr);

			Log::Trace("", "", "时间差天数语句 sqlstr：{0}", sqlstr);
			cmd_inq.ExecuteReader();

			if (cmd_inq.Read())
			{
				ttmsm68["TOTAL_DAYS"] = cmd_inq.GetString(1);
			}
			else
			{
				ttmsm68["TOTAL_DAYS"] = "0";
			}
			cmd_inq.Close();
			Log::Trace("", "", "时间差天数计算值 ttmsm68.TOTAL_DAYS：{0}", ttmsm68["TOTAL_DAYS"].ToString());

			/* 下线 */
			ttmsm68["REC_REVISOR"] = s.userid;   //记录修改责任者
			ttmsm68["REC_REVISE_TIME"] = datetime;   //记录修改时刻

			//sqlstr = "UPDATE TTMSM68 SET REC_REVISOR= '" + ttmsm68["REC_REVISOR"].ToString() + "',"
			//	"REC_REVISE_TIME = '" + ttmsm68["REC_REVISE_TIME"].ToString() + "',"
			//	"CC_MACH_NO= '" + ttmsm68["CC_MACH_NO"].ToString() + "',"					/*铸机号*/
			//	"STRAND_NO= '" + ttmsm68["STRAND_NO"].ToString() + "',"						//流号
			//	"DEV_NO= '" + ttmsm68["DEV_NO"].ToString() + "',"							//设备编号
			//	"ON_LINE_TIME= '" + ttmsm68["ON_LINE_TIME"].ToString() + "',"				//上线时间
			//	"OFF_LINE_TIME= '" + ttmsm68["OFF_LINE_TIME"].ToString() + "',"				//下线时间
			//	"DAYS= " + ttmsm68["DAYS"].ToString() + ","									//计划时间
			//	"TOTAL_DAYS= " + ttmsm68["TOTAL_DAYS"].ToString() + ","						//使用天数
			//	"PLAN_RT = " + ttmsm68["PLAN_RT"].ToString() + ","							//计划过钢量
			//	"PLAN_HAPPEN_WGT = " + ttmsm68["PLAN_HAPPEN_WGT"].ToString() + ","			//过钢量
			//	"STATUS = '" + ttmsm68["STATUS"].ToString() + "',"							//状态
			//	"REMARK_3 = '" + ttmsm68["REMARK_3"].ToString() + "',"						//铸机号
			//	"REMARK_4 = '" + ttmsm68["REMARK_4"].ToString() + "',"						//流号
			//	"REMARK_5 = '" + ttmsm68["REMARK_5"].ToString() + "',"						//状态
			//	"REMARK_2 = '" + ttmsm68["REMARK_2"].ToString() + "'"						//备注
			//	" WHERE SEQ_NO = " + ttmsm68["SEQ_NO"].ToString() + "";
			sqlstr = "UPDATE TTMSM68 SET REC_REVISOR= '" + ttmsm68["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm68["REC_REVISE_TIME"].ToString() + "',"
				"TOTAL_DAYS= " + ttmsm68["TOTAL_DAYS"].ToString() + ","						//使用天数
				"STATUS = '2',REMARK_5 = '离线'"											//2表示离线					//状态
				" WHERE SEQ_NO = " + ttmsm68["SEQ_NO"].ToString() + "";
			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);

			//下线的同时增加新一个设备
			ttmsm68["ON_LINE_TIME"] = datetime;
			ttmsm68["REC_CREATOR"] = s.userid;
			ttmsm68["REC_CREATE_TIME"] = datetime;
			ttmsm68["OFF_LINE_TIME"] =  " ";
			ttmsm68["STATUS"] = "1";
			ttmsm68["REMARK_5"] = "离线";
			ttmsm68["PLAN_HAPPEN_WGT"] = 0;
			ttmsm68["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_68", conn));
			
			//switch (conn->DatabaseKind)
			//{
			//case DB_KIND_DB2:	        // DB2 数据库（未开Oracle兼容）
			//case DB_KIND_DB2_ORACLE:	// DB2 数据库（开Oracle兼容）
			//case DB_KIND_MSSQL:	        // MS SQL Server数据库
			//case DB_KIND_ORACLE:	    // Oracle 数据库
			//default:
			//	sqlstr = "select CODE_DESC_1_CONTENT FROM TSI00FORMCODESETS WHERE FORM_NAME='TMSME68' AND CODE_CLASS='STATUS' AND CODE = @status_no";
			//	break;
			//}
			//cmd_inq.Parameters.Clear();
			//cmd_inq.Parameters.Set("status_no", ttmsm68["STATUS"].ToString());
			//cmd_inq.SetCommandText(sqlstr);
			//cmd_inq.ExecuteReader();

			//if (cmd_inq.Read())
			//{
			//	ttmsm68["REMARK_5"] = cmd_inq.GetString(1);
			//}
			//else
			//{
			//	ttmsm68["REMARK_5"] = " ";
			//}
			//cmd_inq.Close();

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


