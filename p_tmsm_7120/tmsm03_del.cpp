/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2013-09-11
功能: 钢包烘烤实绩删除
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"
#include "ttmsm03.h"

/*<remark>=========================================================
/// <summary>
/// 钢包烘烤实绩删除
/// <para>
/// 1.接收传入的钢包烘烤实绩信息；
/// 2.删除钢包烘烤实绩信息；
/// </para>
/// <para>数据库表：TTMSM03(钢包烘烤实绩表)      </para>
/// <para>主调用函数：前台TTMSM01画面烘烤实绩删除调用；TTMSM03画面F5(删除)调用。   </para>
/// </summary>
/// <param name="bcls_rec">删除的钢包烘烤实绩信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm03_del)

int f_tmsm03_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
{
	/*打印程序起止LOG*/
	CTracer log(__FUNCTION__);

    /* 程序内部变量 */	
	int doFlag = 0;	//返回值
	int count = 0;

	/*数据库SQL操作字符串，用于捕获数据库操作异常情况*/
	CString sqlstr = "";
	
	/* 业务变量 */

	/* 实体类定义 */
	CTTMSM03 ttmsm03(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{		
		//记录总条数
		int rows = bcls_rec->Tables[0].Rows.get_Count(); 
		
		//循环处理
		for (int i = 0; i < rows ; i++ )
		{
			ttmsm03.Reset();
			ttmsm03.MergeFrom(bcls_rec->Tables[0].Rows[i]);
			
			Log::Trace("","tmsm03_del","ttmsm03.BAKE_SEQ_NO [{0}]"	,ttmsm03.BAKE_SEQ_NO);
			Log::Trace("","tmsm03_del","ttmsm03.LADLE_NO [{0}]"		,(const char*)ttmsm03.LADLE_NO);
			Log::Trace("","tmsm03_del","ttmsm03.SM_UNIT_NO [{0}]"	,(const char*)ttmsm03.SM_UNIT_NO);

			// 判传入的参数是否有错	
			if(ttmsm03.BAKE_SEQ_NO <= 0)
			{
				sprintf	(s.msg , "传入的烘烤流水号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_del");
			}
			if(ttmsm03.SM_UNIT_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的炼钢单元号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_del");
			}
			if(ttmsm03.LADLE_NO.Trim() == "")
			{
				sprintf	(s.msg , "传入的钢包号不能为空") ;
				throw	CApplicationException(-1 , s.msg , "tmsm03_del");
			}
			
			/*查询钢包烘烤实绩表*/
			sqlstr = "  SELECT COUNT(1) "
				     "  FROM TTMSM03 "
					 "  WHERE BAKE_SEQ_NO	= @ttmsm03.BAKE_SEQ_NO "
					 "  AND SM_UNIT_NO		= @ttmsm03.SM_UNIT_NO "
					 "  AND LADLE_NO		= @ttmsm03.LADLE_NO "
					 ;
			cmd_inq.SetCommandText( sqlstr );
			cmd_inq.Parameters.Set( "ttmsm03.BAKE_SEQ_NO"	, ttmsm03.BAKE_SEQ_NO );
			cmd_inq.Parameters.Set( "ttmsm03.LADLE_NO"		, ttmsm03.LADLE_NO );
			cmd_inq.Parameters.Set( "ttmsm03.SM_UNIT_NO"	, ttmsm03.SM_UNIT_NO );
			cmd_inq.ExecuteReader();
			count = 0 ;
			if(cmd_inq.Read())
			{
				count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
			}
			Log::Trace("","tmsm03_del","count [{0}]",count );
			cmd_inq.Close();
			if	(count == 0)
			{
				sprintf	(s.msg ,"钢包烘烤实绩信息不存在");
				/*新增钢包烘烤实绩信息报错信息档*/
				throw	CApplicationException(-1 , s.msg , "tmsm03_del");
			}
			
			//根据传入信息删除钢包烘烤实绩表
			sqlstr = "DELETE TTMSM03";
			ttmsm03.Delete(" BAKE_SEQ_NO,SM_UNIT_NO,LADLE_NO ");
		}

		//strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);					
	}
	catch(CDbException& ex)  //捕获数据库操作异常
	{
		CFormattable arguments[] = { ex.GetCode() };
		CMessageFormat::Format(s.msg,  _RES("GCRSS0000006")/*数据库处理出错，sqlcode=[{0}]。请联系系统维护人员。*/, arguments, 1);
		//CMessageFormat::Format(s.msg,  _RES("读取数据失败,表[{0}],sqlcode=[{1}]。请联系系统维护人员."), arguments, 2);
		CString str = sqlstr + "\r\n" + ex.GetMsg();
		strncpy(s.sysmsg, (const char*)str, sizeof(s.sysmsg)-1);
		s.flag = -1;
		doFlag = -1;      //数据库异常时返回-1，事务将被回滚
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
	
	return doFlag;
}