/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-24 10:20:02
Description: RH真空槽信息修改
**************************************************/

#include "stdafx.h"

BM2F_ENTERACE(tmsme69a1_upd)


int f_tmsme69a1_upd(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");

	/* 实体类定义 */
	//CModel ttmsm66 = CModel("TTMSM66");
	CModel ttmsm69("TTMSM69");

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
			ttmsm69.Reset();
			ttmsm69.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm69.TrimOrBlank();

			Log::Trace("", __FUNCTION__, "ttmsm69.HEAT_NO		= [{0}]", (const char*)ttmsm69["HEAT_NO"].ToString());

			if (ttmsm69["HEAT_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "熔炼号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			/* 查询该熔炼号是否存在 */
			if (ttmsm69.QueryCount("HEAT_NO") <= 0)
			{
				sprintf(s.msg, "设熔炼号[%s]不存在!", (const char*)ttmsm69["HEAT_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			//------------------------------------------------------------------
			//ttmsm91["SEQ_NO"] = atoi(EPGetNextSeq("TMSME_91", conn));

			/* 修改工器具RH真空槽信息表 */
			ttmsm69["REC_REVISOR"] = s.userid;   //记录修改责任者
			ttmsm69["REC_REVISE_TIME"] = datetime;   //记录修改时刻

			sqlstr = "UPDATE TTMSM69 SET REC_REVISOR= '" + ttmsm69["REC_REVISOR"].ToString() + "',"
				"REC_REVISE_TIME = '" + ttmsm69["REC_REVISE_TIME"].ToString() + "',"
				"VES_NO_U= " + ttmsm69["VES_NO_U"].ToString() + ","					/*上部槽号*/
				"VES_TIMES_U= " + ttmsm69["VES_TIMES_U"].ToString() + ","				//上部槽龄
				"VES_NO_D= " + ttmsm69["VES_NO_D"].ToString() + ","					//下部槽号
				"VES_TIMES_D= " + ttmsm69["VES_TIMES_D"].ToString() + ","				//下部槽龄
				"USE_SUM= " + ttmsm69["USE_SUM"].ToString() + ","						//浸渍管龄
				"VESSEL_TALL= '" + ttmsm69["VESSEL_TALL"].ToString() + "',"				//脱气时间
				"VES_DEGAS_U= " + ttmsm69["VES_DEGAS_U"].ToString() + ","				//上部真空槽脱气时间
				"VES_DEGAS_D = " + ttmsm69["VES_DEGAS_D"].ToString() + ","			//下部真空槽脱气时间
				"MANUFAC_NAME = '" + ttmsm69["MANUFAC_NAME"].ToString() + "',"			//厂家
				"S_DATETIME = '" + ttmsm69["S_DATETIME"].ToString() + "',"				//喷补开始时间
				"E_DATETIME = '" + ttmsm69["E_DATETIME"].ToString() + "',"				//喷补结束时间
				"SPURT_RESULT = '" + ttmsm69["SPURT_RESULT"].ToString() + "',"			//喷补效果
				"SPURT_END_TIME = '" + ttmsm69["SPURT_END_TIME"].ToString() + "',"		//喷补耗时
				"SPURT_MAT_WT = " + ttmsm69["SPURT_MAT_WT"].ToString() + ","			//喷补量
				"GRP_NO = '" + ttmsm69["GRP_NO"].ToString() + "',"						//班别号
				"SHIFT_NO = '" + ttmsm69["SHIFT_NO"].ToString() + "',"					//班次号
				"RESP= '" + ttmsm69["RESP"].ToString() + "' "							/*责任人*/
				" WHERE HEAT_NO = '" + ttmsm69["HEAT_NO"].ToString() + "'";

			Log::Trace("", "", "sqlstr：{0}", sqlstr);
			Db::Execute(sqlstr);

		

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


