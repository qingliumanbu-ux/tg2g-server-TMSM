/*************************************************
版权: Baosight Software LTD.co Copyright (c) 2012
作者: 179975
日期: 2014-01-20
功能: 铁包包龄计算公式删除
修改历史:
	日期:________;修改人:________; 需求提出人:________
	变更内容:
	
**************************************************/

#include "stdafx.h"

//#include "ttmsm99.h"

/*<remark>=========================================================
/// <summary>
/// 删除
/// <para>
/// 1.接收传入的铁包包龄计算公式信息；
/// 2.删除铁包包龄计算公式信息；
/// </para>
/// <para>数据库表：TTMSM99(铁包包龄计算公式表)      </para>
/// <para>主调用函数：前台TTMSM11画面删除调用。   </para>
/// </summary>
/// <param name="bcls_rec">删除的铁包包龄计算公式信息  </param>
/// <returns>成功：0</returns>
/// <returns>失败：-1</returns>
===========================================================</remark>*/
BM2F_ENTERACE(tmsm99_del)

int f_tmsm99_del(EIClass * bcls_rec, EIClass * bcls_ret, CDbConnection * conn) 
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
	CModel ttmsm11("TTMSM11");
	//CTTMSM99 ttmsm99(conn);
	
	/* 数据库操作类定义 */
	CDbCommand cmd_inq(conn);

	//定义变量--取系统当前时间
	CString datetimeNow = CDateTime::Now().ToString("yyyyMMddHHmmss");

	try
	{		
		////记录总条数
		//int rows = bcls_rec->Tables[0].Rows.get_Count(); 
		//
		////循环处理
		//for (int i = 0; i < rows ; i++ )
		//{
		//	ttmsm99.Reset();
		//	ttmsm99.MergeFrom(bcls_rec->Tables[0].Rows[i]);
		//	
		//	Log::Trace("","tmsm99_del","ttmsm99.MODEL_ID [{0}]"		,(const char*)ttmsm99.MODEL_ID);

		//	// 判传入的参数是否有错	
		//	if(ttmsm99.MODEL_ID.Trim() == "")
		//	{
		//		sprintf	(s.msg , "传入的公式号不能为空") ;
		//		throw	CApplicationException(-1 , s.msg , "tmsm99_del");
		//	}
		//	
		//	/*查询铁包维修实绩表*/
		//	sqlstr = "  SELECT COUNT(1) "
		//		     "  FROM TTMSM11 "
		//			 "  WHERE MODEL_ID	= @ttmsm99.MODEL_ID "
		//			 ;
		//	cmd_inq.SetCommandText( sqlstr );
		//	cmd_inq.Parameters.Set( "ttmsm99.MODEL_ID"	, ttmsm99.MODEL_ID );
		//	cmd_inq.ExecuteReader();
		//	count = 0 ;
		//	if(cmd_inq.Read())
		//	{
		//		count	= cmd_inq.GetInt16(1); //将数据获取到实体对象中				
		//	}
		//	Log::Trace("","tmsm99_del","count {0}",count );
		//	cmd_inq.Close();
		//	if	(count > 0)
		//	{
		//		sprintf	(s.msg ,"还有铁水包使用了该铁包包龄计算公式，不能删除");
		//		/*新增铁包包龄计算公式信息报错信息档*/
		//		throw	CApplicationException(-1 , s.msg , "tmsm99_del");
		//	}
		//	
		//	//根据传入信息删除铁包维修实绩表
		//	sqlstr = "DELETE TTMSM99";
		//	ttmsm99.Delete(" MODEL_ID ");
		//}

		////strcpy(s.msg,_RES("处理成功。")/*处理成功。*/);
		//strcpy(s.msg,_RES("GCRSS0000002")/*处理成功。*/);					
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
