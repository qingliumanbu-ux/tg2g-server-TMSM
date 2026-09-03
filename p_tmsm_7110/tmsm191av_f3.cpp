/*************************************************
Copyright: Baosight Software LTD.co Copyright (c) 2021
Author:      gongnn
Version:     1.0
Date:        2024-03-26
Description: 连铸机设备周期管理弯曲段记录保存
**************************************************/
//框架头文件
#include "stdafx.h"

#include "CUtils.h"



BM2F_ENTERACE(tmsm191av_f3)

int f_tmsm191av_f3(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn)
{
	CTracer log(__FUNCTION__);
	int doFlag = 0;
	CString sqlstr = " ";
	CString s_userid("");
	CString	datetime("");
	CModel ttmsm191("TTMSM191");
	CDbCommand cmd(conn);
	CDbCommand cmd1(conn);
	try
	{
		datetime = CDateTime::Now().ToString("yyyyMMddHHmmss");
		for (int i = 0; i < bcls_rec->Tables[0].Rows.get_Count(); i++)
		{
			ttmsm191.Reset();
			ttmsm191.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			
		
			
			//查询是否有该日期和序号的记录，有记录更新，没有记录新增
			sqlstr = "SELECT count(*) FROM TTMSM191 WHERE DATE_C = @DATE_C "
				" AND  SEQ_NO= @SEQ_NO ";
			cmd1.SetCommandText(sqlstr);
			cmd1.Parameters.Set("DATE_C", ttmsm191["DATE_C"].ToString());
			cmd1.Parameters.Set("SEQ_NO", ttmsm191["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "seq_no= {0}", ttmsm191["SEQ_NO"].ToString());
		    Log::Trace("", __FUNCTION__, "date_ccc= [{0}][{1}]", ttmsm191["DATE_C"].ToString(), ttmsm191["SEQ_NO"].ToString());
			Log::Trace("", __FUNCTION__, "wefgn= [{0}][{1}]", bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString());
			
			CDecimal con = (cmd1.ExecuteScalar()).ToInt32();

			if (con>0)
			{
				ttmsm191["REC_REVISOR"] = s.userid;			//记录修改责任者
				ttmsm191["REC_REVISE_TIME"] = datetime;		//记录修改时刻
				ttmsm191.Update("SERIES_DESC,BEND_NO,CURRENT_STATUS,LOCATION,TOTAL_FURNACE_COUNT,NOTE,REC_REVISOR,REC_REVISE_TIME", "DATE_C,SEQ_NO");

				

			}
			else
			{
				if (bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().Trim() == "" && bcls_rec->Tables[0].Rows[0]["DATE_C"].ToString().Trim() == "")
				{

					ttmsm191["DATE_C"] = datetime.SubstringNE(0, 8);//填表日期为当前日期

				}
				else
				{
					ttmsm191["DATE_C"] = bcls_rec->Tables[0].Rows[0]["QUERY_DATE_C"].ToString().SubstringNE(0, 10).Remove("-");//填表日期为查询日期

				}
				ttmsm191["SEQ_NO"] = bcls_rec->Tables[0].Rows[i]["SEQ_NO"].ToString();//序号
				ttmsm191["REC_CREATOR"] = s.userid;			//记录创建责任者
				ttmsm191["REC_CREATE_TIME"] = datetime;		//记录创建时刻
				ttmsm191.Insert();

			}
		}

	}


	catch (CDbException& ex)
	{
		CFormattable arguments[] = { ex.GetCode(), ex.GetMsg() };
		CMessageFormat::Format(s.msg, "Database Error,sqlcode=[{0}],sqlmsg=[{1}]", arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg) - 1);
		s.flag = -1;
		doFlag = -1;
	}
	catch (CApplicationException& ex)
	{
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	catch (CException& ex)
	{
		strcpy(s.msg, ex.GetMsg());
		s.flag = ex.GetCode();
		doFlag = -1;
	}
	return doFlag;
}


