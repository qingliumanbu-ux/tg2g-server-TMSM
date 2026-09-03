/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2010
Author:    杨扬
Version:   1.0
Date:      2014-07-07
Description: 钢包配包信息_查询
**************************************************/

//框架公用头文件，勿删
#include "stdafx.h"






/*<remark>=========================================================
/// <summary>
///  钢包配包信息_查询
/// <para>
/// 根据输入参数“炼钢计划号”“钢包号”查询《炼钢计划表》《钢包基本信息表》《钢包配包信息表》中的相应信息。
/// <para>数据库表： ttmsm01   钢包基本信息表 </para>
/// <para>数据库表： ttmsm02   钢包配包信息表 </para>
/// <para>数据库表： tpssm11   炼钢计划信息表 </para>
/// <summary>
===========================================================</remark>*/
// service入口
BM2F_ENTERACE(tmsm01a3_inq)

int f_tmsm01a3_inq(EIClass * bcls_rec, EIClass * bcls_ret,CDbConnection * conn) 
{
	CTracer log(__FUNCTION__);
	
	/* 业务变量 */
	CString	datetime("");	
	CString	datetemp("");
	CDecimal stop_use_time = 0;

	CString	sm_plan_no("");
	CString	furnace_no_bof("");
	CString	furnace_no_lf("");

	/* 数据库SQL操作字符串 */
	CString sqlstr     = "" ;
	CString sql_pssm13 = "" ;
	CString sql_tmsm01 = "" ;
	CString sql_tmsm02 = "" ;
	CString sql_pssm_no1 = "" ;
	CString sql_pssm_no2 = "" ;

	/* 数据库操作类定义 */
	CDbCommand cmd(conn);
	CDbCommand cmd_inq(conn);

	/* 实体类定义 */ 
	CModel ttmsm01("TTMSM01");
	CModel ttmsm02("TTMSM02");
	CModel tpssm11("TPSSM11");
	CModel tpssm12("TPSSM12");
	CModel tpssmd1("TPSSMD1");

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* ***** 获取输入参数 ***** */
		if (bcls_rec->Tables[0].Columns.Contains("LADLE_NO"))
		{
			ttmsm01["LADLE_NO"] = bcls_rec->Tables[0].Rows[0]["LADLE_NO"];
			ttmsm02["LADLE_NO"] = bcls_rec->Tables[0].Rows[0]["LADLE_NO"];
		}
		if (bcls_rec->Tables[0].Columns.Contains("SM_HEAT_NO"))
		{
			ttmsm02["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["SM_HEAT_NO"];
			tpssm11["HEAT_NO"] = bcls_rec->Tables[0].Rows[0]["SM_HEAT_NO"];
		}


		ttmsm01["SM_UNIT_NO"] = "A";
		ttmsm02["SM_UNIT_NO"] = "A";

		// 查询数据
		// 1.未配包炼钢计划信息
		// 2.已配包炼钢计划信息
		// 3.可用钢包信息
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	            // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = " SELECT  a.*       "
						 " FROM    TPSSM11 a "
						 " WHERE   a.PONO_STATUS < '83' "
						 ;

				if (tpssm11["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " AND a.HEAT_NO = @tpssm11.HEAT_NO ";
				}

				sql_pssm13 = sqlstr + " AND a.LADLE_NO   = ' ' ";								
				
				break;
		}

		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("tpssm11.HEAT_NO"	,tpssm11["HEAT_NO"].ToString());
		cmd_inq.SetCommandText(sql_pssm13);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[0]);

		bcls_ret->Tables[0].Columns.Add(DT_STRING,"FURNACE_NO_BOF");
		bcls_ret->Tables[0].Columns.Add(DT_STRING,"FURNACE_NO_LF");

		for(int i = 0;i < bcls_ret->Tables[0].Rows.get_Count();i++)
		{
			sm_plan_no = bcls_ret->Tables[0].Rows[i]["SM_PLAN_NO"].ToString().Trim();

			switch(conn->DatabaseKind)
			{
				case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
				case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
				case DB_KIND_MSSQL:	            // MS SQL Server数据库
				case DB_KIND_ORACLE:	        // Oracle 数据库
					sqlstr = " SELECT  a.DEV_CODE                     "
							 " FROM    TPSSM12 a                      "  
							 " WHERE   a.SM_PLAN_NO    =  @sm_plan_no "
							 ;

					sql_pssm_no1 = sqlstr + " AND a.AREA_ID   = '3' ";								
					sql_pssm_no2 = sqlstr + " AND a.AREA_ID   = '4' ";

					break;
			}

			cmd_inq.Parameters.Clear();
			cmd_inq.Parameters.Set("sm_plan_no"	,sm_plan_no);
			cmd_inq.SetCommandText(sql_pssm_no1);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				furnace_no_bof = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			cmd_inq.SetCommandText(sql_pssm_no2);
			cmd_inq.ExecuteReader();
			if(cmd_inq.Read())
			{
				furnace_no_lf = cmd_inq.GetString(1);
			}
			cmd_inq.Close();
			bcls_ret->Tables[0].Rows[i]["FURNACE_NO_BOF"]  = furnace_no_bof;
			bcls_ret->Tables[0].Rows[i]["FURNACE_NO_LF"]   = furnace_no_lf;
		}

		//查询可用钢包信息
		/* 查询钢包信息表 */
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	            // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = " SELECT  t.*       "
						 " FROM    TTMSM01 t "
						 ;
					
				if (ttmsm01["SM_UNIT_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " WHERE t.SM_UNIT_NO = @ttmsm01.SM_UNIT_NO ";
				}
				if (ttmsm01["LADLE_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " AND t.LADLE_NO   = @ttmsm01.LADLE_NO ";
				}

				sql_tmsm01 = sqlstr;								
				
				break;
		}
		sql_tmsm01 = sqlstr;

		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm01.SM_UNIT_NO"	,ttmsm01["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm01.LADLE_NO"	,ttmsm01["LADLE_NO"].ToString());
		cmd_inq.SetCommandText(sql_tmsm01);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[1]);
		cmd_inq.Close();

		for(int i = 0;i < bcls_ret->Tables[1].Rows.get_Count();i++)
		{
			ttmsm01.MergeFrom(bcls_ret->Tables[1].Rows[i]);

			//--------------钢包等级判定开始--------------//
			if((ttmsm01["LADLE_LIFE"].ToDecimal() == 1)&&(ttmsm01["LADLE_LEVEL"].ToString().Trim() == ""))  //新包
			{
				ttmsm01["LADLE_LEVEL"] = "BXXX";
				ttmsm01["LD_STATUS"]   = "3";
			}
			else
			{			
				if(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "D")
				{
					ttmsm01["LADLE_LEVEL"] = ttmsm01["LADLE_LEVEL"];
					ttmsm01["LD_STATUS"]   = ttmsm01["LD_STATUS"];
				}
				else if((ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "A")||
						(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "B")||
						(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")  )
				{
					//获取钢包最近一次操作基准时间，若是 新包 或 小修中修后的钢包 烘烤后第一次使用，则取烘烤结束时间，否则取上次使用结束时间
					//if(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")   //烘烤后赋C,使用后赋A
					//{
					//	datetemp = ttmsm01["DRYING_ET"];
					//}
					//else
					//{
					//	datetemp = ttmsm01["USAGE_ET"];
					//}

					//CDateTime dt1 = CDateTime:: Parse(datetime);
					//CDateTime dt3 = CDateTime:: Parse(datetemp);

					//CTimeSpan ts1 = dt1 - dt3;
					//stop_use_time = ts1.Days() * 24 * 60 + ts1.Hours() * 60 + ts1.Minutes();

					if((stop_use_time <= 45)&&(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "A"))
					{
						ttmsm01["LADLE_LEVEL"] = "A";
						ttmsm01["LD_STATUS"]   = "0";
					}
					if((stop_use_time <= 45)&&(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")) //维修或新包烤包第一次使用，C为烘烤时赋值
					{
						ttmsm01["LADLE_LEVEL"] = "C" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "0";
					}
					if((stop_use_time > 45)&&(stop_use_time <= 70))
					{
						ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "0";
					}
					if((stop_use_time > 70)&&(stop_use_time <= 150))
					{
						ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "1";
					}
					if((stop_use_time > 150)&&(stop_use_time <= 999))
					{
						ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "2";
					}
					if(stop_use_time > 999)
					{
						ttmsm01["LADLE_LEVEL"] = "BXXX";
						ttmsm01["LD_STATUS"]   = "2";
					}
				}
				else
				{
					/*sprintf(s.msg,"钢包等级[%s]错误!",(const char*)ttmsm01["LADLE_LEVEL"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);*/

					ttmsm01["LADLE_LEVEL"] = "BXXX";
					ttmsm01["LD_STATUS"]   = "2";
				}
			}
			//--------------钢包等级判定结束--------------//

			bcls_ret->Tables[1].Rows[i]["LADLE_LEVEL"] = ttmsm01["LADLE_LEVEL"];
			bcls_ret->Tables[1].Rows[i]["LD_STATUS"]   = ttmsm01["LD_STATUS"];

			//ttmsm01.Update("LADLE_LEVEL,LD_STATUS","LADLE_NO");
			
		}

		//查询钢包配包计划信息
		switch(conn->DatabaseKind)
		{
			case DB_KIND_DB2:	            // DB2 数据库（未开Oracle兼容）
			case DB_KIND_DB2_ORACLE:	    // DB2 数据库（开Oracle兼容）
			case DB_KIND_MSSQL:	            // MS SQL Server数据库
			case DB_KIND_ORACLE:	        // Oracle 数据库
				sqlstr = " SELECT  t.*       "
						 " FROM    TTMSM02 t "
						 ;
					
				if (ttmsm02["SM_UNIT_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " WHERE t.SM_UNIT_NO = @ttmsm02.SM_UNIT_NO ";
				}

				if (ttmsm02["HEAT_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " AND t.HEAT_NO = @ttmsm02.HEAT_NO ";
				}
				if (ttmsm02["LADLE_NO"].ToString().Trim() != "")
				{
					sqlstr = sqlstr + " AND t.LADLE_NO   = @ttmsm02.LADLE_NO ";
				}

				sql_tmsm02 = sqlstr;								
				
				break;
		}
		sql_tmsm02 = sqlstr;

		bcls_ret->Tables.Add();
		cmd_inq.Parameters.Clear();
		cmd_inq.Parameters.Set("ttmsm02.SM_UNIT_NO"	,ttmsm02["SM_UNIT_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm02.HEAT_NO"	,ttmsm02["HEAT_NO"].ToString());
		cmd_inq.Parameters.Set("ttmsm02.LADLE_NO"	,ttmsm02["LADLE_NO"].ToString());
		cmd_inq.SetCommandText(sql_tmsm02);
		cmd_inq.ExecuteQuery(bcls_ret->Tables[2]);
		cmd_inq.Close();
		bcls_ret->Tables[2].Columns.Add(DT_DECIMAL, "LADLE_LIFE");
		bcls_ret->Tables[2].Columns.Add(DT_STRING, "USE_REMARK");

		for(int i = 0;i < bcls_ret->Tables[2].Rows.get_Count();i++)
		{
			ttmsm02.MergeFrom(bcls_ret->Tables[2].Rows[i]);

			ttmsm01["LADLE_NO"] = ttmsm02["LADLE_NO"];
			ttmsm01.Query("LADLE_NO");
			//--------------钢包等级判定开始--------------//
			if((ttmsm01["LADLE_LIFE"].ToDecimal() == 1)&&(ttmsm01["LADLE_LEVEL"].ToString().Trim() == ""))  //新包
			{
				ttmsm01["LADLE_LEVEL"] = "BXXX";
				ttmsm01["LD_STATUS"]   = "3";
			}
			else
			{			
				if(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "D")
				{
					ttmsm01["LADLE_LEVEL"] = ttmsm01["LADLE_LEVEL"];
					ttmsm01["LD_STATUS"]   = ttmsm01["LD_STATUS"];
				}
				else if((ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "A")||
						(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "B")||
						(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")  )
				{
					//获取钢包最近一次操作基准时间，若是 新包 或 小修中修后的钢包 烘烤后第一次使用，则取烘烤结束时间，否则取上次使用结束时间
					//if(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")   //烘烤后赋C,使用后赋A
					//{
					//	datetemp = ttmsm01["DRYING_ET"];
					//}
					//else
					//{
					//	datetemp = ttmsm01["USAGE_ET"];
					//}

					//CDateTime dt1 = CDateTime:: Parse(datetime);
					//CDateTime dt3 = CDateTime:: Parse(datetemp);

					//CTimeSpan ts1 = dt1 - dt3;
					//stop_use_time = ts1.Days() * 24 * 60 + ts1.Hours() * 60 + ts1.Minutes();

					if((stop_use_time <= 45)&&(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "A"))
					{
						ttmsm01["LADLE_LEVEL"] = "A";
						ttmsm01["LD_STATUS"]   = "0";
					}
					if((stop_use_time <= 45)&&(ttmsm01["LADLE_LEVEL"].ToString().Substring(0,1) == "C")) //维修或新包烤包第一次使用，C为烘烤时赋值
					{
						ttmsm01["LADLE_LEVEL"] = "C" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "0";
					}
					if((stop_use_time > 45)&&(stop_use_time <= 70))
					{
						ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "0";
					}
					if((stop_use_time > 70)&&(stop_use_time <= 150))
					{
						ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "1";
					}
					if((stop_use_time > 150)&&(stop_use_time <= 999))
					{
						ttmsm01["LADLE_LEVEL"] = "B" + stop_use_time.ToString().Trim();
						ttmsm01["LD_STATUS"]   = "2";
					}
					if(stop_use_time > 999)
					{
						ttmsm01["LADLE_LEVEL"] = "BXXX";
						ttmsm01["LD_STATUS"]   = "2";
					}
				}
				else
				{
					/*sprintf(s.msg,"钢包等级[%s]错误!",(const char*)ttmsm01["LADLE_LEVEL"].ToString());
					throw CApplicationException(-1, s.msg, s.svc_name);*/

					ttmsm01["LADLE_LEVEL"] = "BXXX";
					ttmsm01["LD_STATUS"]   = "2";
				}
			}
			//--------------钢包等级判定结束--------------//

			bcls_ret->Tables[2].Rows[i]["LADLE_LIFE"]  = ttmsm01["LADLE_LIFE"];
			bcls_ret->Tables[2].Rows[i]["LADLE_LEVEL"] = ttmsm01["LADLE_LEVEL"];
			bcls_ret->Tables[2].Rows[i]["USE_REMARK"]  = ttmsm01["USE_REMARK"];
			
			//ttmsm02.Update("LADLE_LEVEL","LADLE_NO");
			
		}

	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch(CApplicationException& ex)  //捕获应用错误
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch(CException& ex)
	{
		strncpy(s.msg, (const char*)ex.GetMsg(), sizeof(s.msg)-1);
		s.flag = ex.GetCode();
		doFlag = -1;
	}

	cmd_inq.Close();

	return doFlag;

}
