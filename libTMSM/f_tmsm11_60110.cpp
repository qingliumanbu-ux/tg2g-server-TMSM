/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2023
Author:      180969
Version:     1.0
Date:        2023-05-31 13:46:39
Description: 更新铁水包状态位置
**************************************************/

#include "stdafx.h"

BM2_FUNCTION_EXPORT

//记录履历
int f_tmsm67_trace(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn);

int f_tmsm11_60110(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);

	/*程序用变量*/
	int doFlag = 0;
	int blkNum = 0;

	/* 业务变量 */
	CString datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
	CString userid = s.userid;  

	CString p_moltiron_tank_no = " ";
	CString p_case = " ";

	/* 实体类定义 */
	//CModel ttmsm01("TTMSM01");

	/* 数据库SQL操作字符串 */
	CString sqlstr;

	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");

		/* 获得传入参数 */
		p_moltiron_tank_no = bcls_rec->Tables[0].Rows[0]["P_MOLTIRON_TANK_NO"].ToString().Trim();  //moltiron_tank_no
		p_case			   = bcls_rec->Tables[0].Rows[0]["P_CASE"].ToString().Trim();			

		Log::Trace("", __FUNCTION__, "需要更新状态的铁水包号 = [{0}]", p_moltiron_tank_no);


		/* 查询该序列号是否存在 */
		//if (ttmsm01.QueryCount("LADLE_NO") <= 0)
		//{
		//	sprintf(s.msg, "钢包号[%s]不存在!", (const char*)ttmsm01["LADLE_NO"].ToString());
		//	throw CApplicationException(-1, s.msg, s.svc_name);
		//}

		//--区域为1#PO
		if (p_case == "19")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '19' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr19：{0}", sqlstr);
			Db::Execute(sqlstr);
		}
		
		//--区域为2#PO
		if (p_case == "20")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '20' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr20：{0}", sqlstr);
			Db::Execute(sqlstr);
		}


		//--区域为1#DS
		if (p_case == "21")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '21' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr21：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		// --区域为2#DS
		if (p_case == "22")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '22' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr22：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		// --区域为3#DS
		if (p_case == "23")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '23' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr23：{0}", sqlstr);
			Db::Execute(sqlstr);
		}
	
		// --区域为1#BOF
		if (p_case == "1")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '1' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr1：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		// --区域为2#BOF
		if (p_case == "1")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '2' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr2：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		// --区域为2#BOF
		if (p_case == "2")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '2' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr2：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		// --区域为3#BOF
		if (p_case == "11")
		{
			sqlstr = "UPDATE TTMSM11 SET REC_REVISOR= '" + userid + "',"
				"REC_REVISE_TIME = '" + datetime + "',"
				"IRON_LADLE_POS= '11' "
				" WHERE IRON_LADLE_NO = '" + p_moltiron_tank_no + "'";

			Log::Trace("", "", "sqlstr3：{0}", sqlstr);
			Db::Execute(sqlstr);
		}

		//抛其它工器具跟踪履历
		if (!bcls_rec->Tables.Contains("TMSM67"))
		{
			bcls_rec->Tables.Add("TMSM67");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "SM_UNIT_NO");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "REMARK_1");
			bcls_rec->Tables["TMSM67"].Columns.Add(DT_STRING, "REMARK");
		}
		bcls_rec->Tables["TMSM67"].Rows.Clear();

		//其它工器具跟踪履历跟踪函数
		bcls_rec->Tables["TMSM67"].Rows.Add();
		bcls_rec->Tables["TMSM67"].Rows[0]["SM_UNIT_NO"] ="Z";				//用67表记录其它工器具履历
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK_1"] = "更新铁水包状态";  //时间操作
		bcls_rec->Tables["TMSM67"].Rows[0]["REMARK"] = sqlstr;				 //事件
		doFlag = f_tmsm67_trace(bcls_rec, bcls_ret, conn);
		if (doFlag < 0)
		{
			throw CApplicationException(-1, s.msg, log.Location);
		}

		
	}
	catch (CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg, "数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。" /* _RES("GCRSS0000006")*//*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);  //返回前台，与EI.EIInfo对象的sys_info.sysmsg参数对应
		//Log::Trace((1,1, "[%s]", s.sysmsg);
		s.flag = -1;
		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚
	}
	catch (CApplicationException& ex)  //捕获应用错误
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
	return doFlag;
}


