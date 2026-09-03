/* ****************************************************************************
 *	Copyright (c) Baosight Corporation 2008 . All Rights Reserved.
 *  	BM2PES 宝信生产执行系统
 *****************************************************************************
 *  程序名称			: f_t8z_23m_snd
 *  程序描述			: 二钢北区向智慧质量发送数据电文
 *  备注说明			:
 *  编制人			：李振
 *  修改历史			: 2025-07-29 	BM2IDE			(ADD)程序建立
 *			... ...
 * **************************************************************************** */
 // 框架公用头文件，勿删
#include "stdafx.h"

// 程序用头文件
#include "epex.h"

// #include "xhmzl06.h"

int f_t8z_23m_snd(EIClass* bcls_rec, EIClass* bcls_ret, CDbConnection* conn)
{
	CTracer log(__FUNCTION__);

	/* ***** 静态变量定义 ***** */
	int doFlag = 0;
	CString lpsz_tc_no = " ";

	EPEX epex;
	CString table_name;
	CString table_name1;
	CString tc_no;

	CDbCommand cmd_inq(conn);
	CString sqlstr;

	try
	{
		/* ***** 获取输入参数 ***** */
		tc_no = bcls_rec->Tables[0].Rows[0]["TC_NO"].ToString();
		if (tc_no == "T82314") // 转炉
		{
			table_name = "TMMSM21";
			table_name1 = "TMMSM11";
			sqlstr = " select * from TMMSM11 where 1=1 and HEAT_NO in (SELECT IRON_LTREAT_NO FROM TMMSM21 WHERE HEAT_NO = '" + bcls_rec->Tables[1].Rows[0]["HEAT_NO"].ToString() + "') ";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			bcls_rec->Tables.Add();
			cmd_inq.ExecuteQuery(bcls_rec->Tables[2]);
			cmd_inq.Close();

			if (bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "B0")
			{
				sqlstr = " select BUNKER_NO,SUM(STOCK_WT) STOCK_WT from TMMSM85 where BUNKER_NO IN ('0B03','0B06','0B08') GROUP BY BUNKER_NO ";
			}
			else if (bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "B1")
			{
				sqlstr = " select BUNKER_NO,SUM(STOCK_WT) STOCK_WT from TMMSM85 where BUNKER_NO IN ('1B16','1B17','1B19') GROUP BY BUNKER_NO ";
			}
			else if (bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "B2")
			{
				sqlstr = " select BUNKER_NO,SUM(STOCK_WT) STOCK_WT from TMMSM85 where BUNKER_NO IN ('2B17') GROUP BY BUNKER_NO ";
			}
			else {
				sqlstr = " ";
			}
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			if (sqlstr.Trim() != "")
			{
				cmd_inq.SetCommandText(sqlstr);
				bcls_rec->Tables.Add("TMMSM85");
				cmd_inq.ExecuteQuery(bcls_rec->Tables["TMMSM85"]);
				cmd_inq.Close();

			}
			
		}
		else if (tc_no == "T82315") // LF
		{
			table_name = "TMMSM24";
		}
		else if (tc_no == "T82317") // AOD
		{
			table_name = "TMMSM27";
		}
		else if (tc_no == "T82316") // RH
		{
			table_name = "TMMSM23";

			if (bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "R1"
				|| bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "R2")
			{
				sqlstr = " select BUNKER_NO,SUM(STOCK_WT) STOCK_WT from TMMSM85 where BUNKER_NO IN ('H11','H12','H13','H14','H15','H16','H17','H18') GROUP BY BUNKER_NO  ORDER BY BUNKER_NO ";
			}
			else if (bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "R3"
				|| bcls_rec->Tables[1].Rows[0]["DEV_CODE"].ToString() == "R4")
			{
				sqlstr = " select BUNKER_NO,SUM(STOCK_WT) STOCK_WT from TMMSM85 where BUNKER_NO IN ('H21','H22','H23','H24','H25','H26','H27','H28') GROUP BY BUNKER_NO ORDER BY BUNKER_NO ";
			}
			else {
				sqlstr = " ";
			}
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			if (sqlstr.Trim() != "")
			{
				cmd_inq.SetCommandText(sqlstr);
				bcls_rec->Tables.Add("TMMSM85");
				cmd_inq.ExecuteQuery(bcls_rec->Tables["TMMSM85"]);
				cmd_inq.Close();

			}
		}
		else if (tc_no == "T82318") // VD
		{
			table_name = "TMMSM25";
		}
		else if (tc_no == "T82321") // 连铸
		{
			table_name = "TMMSM31";
		}
		else if (tc_no == "T82319") // 脱硫
		{
			table_name = "TMMSMKR14";
		}
		else if (tc_no == "T82322") // 主档
		{
			table_name = "TMMSM01";
		}
		else if (tc_no == "T82320") // 投料
		{
			table_name = "TMMSM2A";
		}
		else if (tc_no == "T82312") // 成分实绩
		{
			table_name = "TQMTS24";
			table_name1 = "TQMTS25";
		}
		else if (tc_no == "T82313") // 成分标准
		{
			table_name = "TQMTS0X";
			table_name1 = "TQMTS02";
		}
		else if (tc_no == "T82324") // 预计划
		{
			table_name = "TPSSM01";
			table_name1 = "TPSSM03";
		}
		else if (tc_no == "T82325") // 计划信号
		{
			table_name = "TPSSM11";
			table_name1 = "TPSSM12";

			sqlstr = " SELECT * FROM TPSSM12 WHERE HEAT_NO = '" + bcls_rec->Tables[1].Rows[0]["HEAT_NO"].ToString() + "' ";
			Log::Trace("", __FUNCTION__, "sqlstr = [{0}]", sqlstr);
			cmd_inq.SetCommandText(sqlstr);
			bcls_rec->Tables.Add();
			cmd_inq.ExecuteQuery(bcls_rec->Tables[2]);
			cmd_inq.Close();

			
		}
		else
		{
			return 0;
		}

		Log::Trace("", __FUNCTION__, "=== ttmsm02.SM_PLAN_NO = [{0}]", table_name);
		CModel tsmxx(table_name);
		CModel tsmxx1(table_name1);

		/* *****	发送电文开始 ***** */
		// 初始化
		if (epex.Initialize(tc_no) < 0)
		{
			strcpy(s.msg, _RES("GCRSS0000007") /*系统出现异常，请联系系统维护人员。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		for (int i = 0; i < bcls_rec->Tables[1].Rows.get_Count(); i++)
		{
			tsmxx.MergeFrom(bcls_rec->Tables[1].Rows[i]);

			if (tc_no == "T82312")
			{
				if (epex.SetValue("tqmts24", i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				for (int j = 0; j < bcls_rec->Tables[2].Rows.get_Count(); j++)
				{
					tsmxx1.MergeFrom(bcls_rec->Tables[2].Rows[j]);
					if (epex.SetValue("tqmts25", j, tsmxx1) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (tc_no == "T82313")
			{
				if (epex.SetValue("T1", i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				for (int j = 0; j < bcls_rec->Tables[2].Rows.get_Count(); j++)
				{
					tsmxx1.MergeFrom(bcls_rec->Tables[2].Rows[j]);
					if (epex.SetValue("T2", j, tsmxx1) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (tc_no == "T82314")
			{
				if (epex.SetValue("tmmsm21", i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				for (int j = 0; j < bcls_rec->Tables[2].Rows.get_Count(); j++)
				{
					tsmxx1.MergeFrom(bcls_rec->Tables[2].Rows[j]);
					if (epex.SetValue("tmmsm11", j, tsmxx1) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
				if (bcls_rec->Tables.Contains("TMMSM85"))
				{
					for (int j = 0; j < bcls_rec->Tables["TMMSM85"].Rows.get_Count(); j++)
					{
						
						if (epex.SetValue("T2","STK_NO", j, bcls_rec->Tables["TMMSM85"].Rows[j]["BUNKER_NO"].ToString()) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						if (epex.SetValue("T2", "STK_NO_WT", j, bcls_rec->Tables["TMMSM85"].Rows[j]["STOCK_WT"].ToDecimal()) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
			}
			else if (tc_no == "T82316")
			{
				if (epex.SetValue("T1",i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				if (bcls_rec->Tables.Contains("TMMSM85"))
				{
					for (int j = 0; j < bcls_rec->Tables["TMMSM85"].Rows.get_Count(); j++)
					{

						if (epex.SetValue("T2", "STK_NO", j, bcls_rec->Tables["TMMSM85"].Rows[j]["BUNKER_NO"].ToString()) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
							throw CApplicationException(-1, s.msg, log.Location);
						}
						if (epex.SetValue("T2", "STK_NO_WT", j, bcls_rec->Tables["TMMSM85"].Rows[j]["STOCK_WT"].ToDecimal()) < 0)
						{
							strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
							throw CApplicationException(-1, s.msg, log.Location);
						}
					}
				}
			}
			else if (tc_no == "T82324")
			{
				if (epex.SetValue("tpssm01", i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				for (int j = 0; j < bcls_rec->Tables[2].Rows.get_Count(); j++)
				{
					tsmxx1.MergeFrom(bcls_rec->Tables[2].Rows[j]);
					if (epex.SetValue("tpssm03", j, tsmxx1) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else if (tc_no == "T82325")
			{
				if (epex.SetValue("T1", i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
				for (int j = 0; j < bcls_rec->Tables[2].Rows.get_Count(); j++)
				{
					tsmxx1.MergeFrom(bcls_rec->Tables[2].Rows[j]);
					if (epex.SetValue("T2", j, tsmxx1) < 0)
					{
						strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
						throw CApplicationException(-1, s.msg, log.Location);
					}
				}
			}
			else
			{
				if (epex.SetValue(i, tsmxx) < 0)
				{
					strcpy(s.msg, _RES("GCRSS0000015") /*系统出现异常，电文拼接出错，请联系系统维护人员。*/);
					throw CApplicationException(-1, s.msg, log.Location);
				}
			}
		}

		// 发送电文
		if (epex.SendTele() < 0)
		{
			sprintf(s.sysmsg, _RES("GCRSS0000032") /*电文发送失败。*/);
			strcpy(s.msg, _RES("GCRSS0000032") /*电文发送失败。*/);
			throw CApplicationException(-1, s.msg, log.Location);
		}
		// 释放
		epex.Uninitialize();

		/* *****	发送电文end ***** */
		doFlag = 0;
		s.sqlcode = 0;
		strcpy(s.msg, _RES("GCRSS0000002")); /*操作成功。*/
	}
	catch (CDbException& ex) // 捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, _RES("GCRSS0000006") /*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1); // 返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		s.flag = -1;
		doFlag = -1; // 数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex) // 捕获应用错误
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
