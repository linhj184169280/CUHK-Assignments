using UnityEngine;
using System.Collections;

public class SpinAircraft : MonoBehaviour {
	public Transform center;


	//public float radius = 30f;
	public float radiusSpeed = 0.5f;
	public float rotationSpeed = 20.0f;
	private Vector3 axis;// = Vector3.up;

	// Use this for initialization
	void Start () {
		//transform.position = (transform.position - center.position).normalized * radius + center.position;
		Vector3 Aircraft_posL = new Vector3 (25.0f, 0.0f, 0.0f);
		transform.position = center.TransformPoint (Aircraft_posL);
		transform.Rotate(new Vector3 (270f, 0f, 0f));
		axis = center.up;
	}
	
	// Update is called once per frame
	void Update () {
		transform.RotateAround (center.position, axis, -rotationSpeed * Time.deltaTime);
		//Vector3 desirePosition =  ( transform.position - center.position).normalized * radius + center.position ;
		//transform.position = Vector3.MoveTowards (transform.position, desirePosition, Time.deltaTime * radiusSpeed);
	
	}
	
}
