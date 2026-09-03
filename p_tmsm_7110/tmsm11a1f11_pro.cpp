/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-06-06 14:30:17
Description: 铁包火焰录入
**************************************************/

#include "stdafx.h"

//外部函数声明
void f_epep_get_shift_group(const CString& strShiftClass, const CString& strShiftTime, CString& strShiftNo, CString& strShiftGroup, CDbConnection * conn); //班次班别函数


BM2F_ENTERACE(tmsm11a1f11_pro)


int f_tmsm11a1f11_pro(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__); 	//系统日志类定义

	/* 程序内部变量 */
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString	datetime("");
	CString strShiftNo("");
	CString strShiftGroup("");

	/* 实体类定义 */
	CModel ttmsm11("TTMSM11");

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
			ttmsm11.Reset();
			ttmsm11.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm11.TrimOrBlank();

			ttmsm11["OP_DIV"] = "1";
			ttmsm11["SM_UNIT_NO"] = "A";

			Log::Trace("", __FUNCTION__, "ttmsm11.SM_UNIT_NO			= [{0}]", (const char*)ttmsm11["SM_UNIT_NO"].ToString());
			Log::Trace("", __FUNCTION__, "ttmsm11.IRON_LADLE_NO		= [{0}]", (const char*)ttmsm11["IRON_LADLE_NO"].ToString());

			/* 检查输入参数合法性 */
			/*if(ttmsm11["SM_UNIT_NO"].ToString().Trim() == "")
			{
			strcpy(s.msg,"炼钢单元号不能为空!");
			throw CApplicationException(-1, s.msg, s.svc_name);
			}*/
			if (ttmsm11["IRON_LADLE_NO"].ToString().Trim() == "")
			{
				strcpy(s.msg, "铁包号不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm11["FLAME_ST"].ToString().Trim() == "")
			{
				strcpy(s.msg, "火焰开始时刻不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}
			if (ttmsm11["FLAME_TP"].ToString().Trim() == "")
			{
				strcpy(s.msg, "火焰类型不能为空!");
				throw CApplicationException(-1, s.msg, s.svc_name);
			}


			/* 查询该钢包号是否存在 */
			if (ttmsm11.QueryCount("SM_UNIT_NO,IRON_LADLE_NO") <= 0)
			{
				sprintf(s.msg, "铁包号[%s]不存在!", (const char*)ttmsm11["IRON_LADLE_NO"].ToString());
				throw CApplicationException(-1, s.msg, s.svc_name);
			}

			


			/* 设置默认值 */
			ttmsm11["MIT_STATUS"] = "01";	//11-烘烤
			f_epep_get_shift_group("SM", datetime, strShiftNo, strShiftGroup, conn);//取得班次班组
			if (ttmsm11["BAKE_SHIFT_NO"].ToString().Trim() == "")
			{
				ttmsm11["BAKE_SHIFT_NO"] = strShiftNo;		//生产班次号
			}
			if (ttmsm11["BAKE_GRP_NO"].ToString().Trim() == "")
			{
				ttmsm11["BAKE_GRP_NO"] = strShiftGroup;		//生产班次号
			}

			if (ttmsm11["BAKE_WRITER"].ToString().Trim() == "")
			{
				ttmsm11["BAKE_WRITER"] = s.userid;
			}

			if (ttmsm11["FLAME_ST"].ToString().Trim() != "" && ttmsm11["FLAME_ET"].ToString().Trim() != "")
			{
				CDateTime dt1 = CDateTime::Parse(ttmsm11["FLAME_ST"].ToString());
				CDateTime dt2 = CDateTime::Parse(ttmsm11["FLAME_ET"].ToString());
				CTimeSpan ts = dt2 - dt1;

				ttmsm11["ELAPSED_TIME"] = ts.Days() * 24 * 60 + ts.Hours() * 60 + ts.Minutes();
				Log::Info("", __FUNCTION__, "持续时间 = [{0}]", ttmsm11["ELAPSED_TIME"].ToDecimal());

			}

			/* 修改铁包信息 */
			ttmsm11["REC_REVISOR"] = s.userid;   //记录创建责任者
			ttmsm11["REC_REVISE_TIME"] = datetime;   //记录创建时刻
			ttmsm11.TrimOrBlank();
			//ttmsm11.Print();
			sqlstr = //"BAKE_SEQ_NO,"
				//"MIT_STATUS,"
				//"BAKE_TYPE,"
				//"BAKE_POS,"
				//"BAKE_START_TEMP,"
				//"BAKE_END_TEMP,"
				"FLAME_ST,"
				"FLAME_ET,"
				//"ELAPSED_TIME,"
				//"BAKE_SHIFT_NO,"
				//"BAKE_GRP_NO,"
				//"BAKE_WRITER,"
				"FLAME_TP,"
				"REC_REVISOR,"
				"REC_REVISE_TIME";
			ttmsm11.Update(sqlstr, "IRON_LADLE_NO");
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


