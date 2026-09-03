/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-02
Description: 保护渣抽检记录导入
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm14av_f3)

int f_tmsm14av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString sqlstr_inq = " ";
	CString s_userid("");
	CString	datetime("");
	CDateTime date_c;
	CModel ttmsm14("TTMSM14");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm14.Reset();
			ttmsm14.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			ttmsm14["REC_CREATOR"] = s.userid;			//记录创建责任者
			ttmsm14["REC_CREATE_TIME"] = datetime;		//记录创建时刻
			ttmsm14["REC_REVISOR"] = s.userid;			//记录修改责任者
			ttmsm14["REC_REVISE_TIME"] = datetime;		//记录修改时刻
			//数据校验等操作可在此进行
			if (ttmsm14["DATE_C"].ToString() == " ")
			{
				strcpy(s.msg, "日期为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm14["C_ORDERID"].ToString() == " ")
			{
				strcpy(s.msg, "订单号为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			if (ttmsm14["SAP_ERP_MATNR"].ToString() == " ")
			{
				strcpy(s.msg, "物料编码为空,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			/*if (ttmsm14["DATE_C"].ToString().Trim() != "")
			{
				date_c = CDateTime::Parse(ttmsm14["DATE_C"].ToString().SubstringNE(0, 8));
			}*/
			sqlstr = "SELECT count(*) FROM TTMSM14 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm14["DATE_C"].ToString());
            cmd1.Parameters.Set("SEQ_NO", ttmsm14["SEQ_NO"].ToString());
		
			
			Log::Trace("", __FUNCTION__, "b		= [{0}][{1}]", ttmsm14["DATE_C"].ToString(), bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToString());
		
			Log::Trace("", __FUNCTION__, "str_sql		= [{0}]", sqlstr);
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();

			if (con>1)
			{
				strcpy(s.msg, "数据已存在,请重新输入");
				s.flag = -1;
				return doFlag;
			}
			
			CDecimal st_seq_no = 1;
			sqlstr_inq = " SELECT MAX(SEQ_NO) FROM TTMSM14";
			Log::Trace("", __FUNCTION__, "str_sql_inq		= [{0}]", sqlstr_inq);
			cmd.SetCommandText(sqlstr_inq);
			cmd.ExecuteReader();
			if (cmd.Read())
			{
				st_seq_no = cmd.GetDecimal(1) + 1;
			}
			ttmsm14["SEQ_NO"] = st_seq_no;		//序号
			ttmsm14["DATE_C"] = ttmsm14["DATE_C"].ToString();		//日期
			
			ttmsm14.Insert();
		}

	}
	catch (const CApplicationException& ex)

	{

		doFlag = ex.GetCode();

		strcpy(s.msg, (const char*)ex.GetMsg());

	}

	catch (CException& ex)  //用于捕获数据库操作异常

	{

		strcpy(s.msg, ex.GetMsg());  //返回前台，与EI.EIManager.Instance.CallService(v_curr_part_name,)方法返回的EI.EIInfo对象的sys_info.msg参数对应

		s.flag = ex.GetCode();       //返回前台，与EI.EIManager.Instance.CallService(v_curr_part_name,)方法返回的EI.EIInfo对象的sys_info.flag参数对应

		doFlag = -1;                 //数据库异常时返回-1，事务将被回滚

	}

	//返回-1时事务将回滚，返回为0是事务将提交

	return(doFlag);
}


